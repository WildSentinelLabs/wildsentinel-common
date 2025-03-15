#pragma once

#include "arch/config.h"
#include "base/delegate.h"
#include "concurrency/detail/allocator_traits.h"
#include "concurrency/detail/atomic_backoff.h"
#include "concurrency/detail/concurrent_monitor.h"
#include "concurrency/detail/helpers.h"
#include "concurrency/spin_mutex.h"

namespace ws {
namespace concurrency {
namespace detail {

using ticket_type = std::size_t;

template <typename TPage>
inline bool IsValidPage(const TPage p) {
  return reinterpret_cast<std::uintptr_t>(p) > 1;
}

template <typename T, typename TAllocator>
struct ConcurrentQueueRep;

template <typename TContainer, typename T, typename TAllocator>
class MicroQueuePopFinalizer;

#if _MSC_VER && !defined(__INTEL_COMPILER)
#pragma warning(push)
#pragma warning(disable : 4146)
#endif

template <typename T, typename TAllocator>
class MicroQueue {
 private:
  using queue_rep_type = ConcurrentQueueRep<T, TAllocator>;
  using self_type = MicroQueue<T, TAllocator>;

 public:
  using size_type = std::size_t;
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;

  using allocator_type = TAllocator;
  using allocator_traits_type = std::allocator_traits<allocator_type>;
  using queue_allocator_type =
      typename allocator_traits_type::template rebind_alloc<queue_rep_type>;

  static constexpr size_type kItemSize = sizeof(T);
  static constexpr size_type kItemsPerPage = kItemSize <= 8     ? 32
                                             : kItemSize <= 16  ? 16
                                             : kItemSize <= 32  ? 8
                                             : kItemSize <= 64  ? 4
                                             : kItemSize <= 128 ? 2
                                                                : 1;
  struct PaddedPage {
    PaddedPage() {}
    ~PaddedPage() {}

    reference operator[](std::size_t index) {
      assert(index < kItemsPerPage && "Index out of range");
      return items[index];
    }

    const_reference operator[](std::size_t index) const {
      assert(index < kItemsPerPage && "Index out of range");
      return items[index];
    }

    PaddedPage* next{nullptr};
    std::atomic<std::uintptr_t> mask{};

    union {
      value_type items[kItemsPerPage];
    };
  };

  using page_allocator_type =
      typename allocator_traits_type::template rebind_alloc<PaddedPage>;

  using item_constructor_type = void (*)(value_type* location, const void* src);
  MicroQueue() = default;
  MicroQueue(const MicroQueue&) = delete;
  MicroQueue& operator=(const MicroQueue&) = delete;

  size_type PreparePage(ticket_type k, queue_rep_type& base,
                        page_allocator_type page_allocator, PaddedPage*& p) {
    assert(p == nullptr && "Invalid page argument for prepare_page");
    k &= -queue_rep_type::kNQueue;
    size_type index = ws::arch::detail::ModulusPowerOfTwo(
        k / queue_rep_type::kNQueue, kItemsPerPage);
    if (!index) {
      ws::concurrency::detail::templates::TryCall([&] {
        p = page_allocator_traits::allocate(page_allocator, 1);
      }).OnException([&] {
        ++base.n_invalid_entries_;
        InvalidatePage(k);
      });
      page_allocator_traits::construct(page_allocator, p);
    }

    SpinWaitUntilTurn(tail_counter_, k, base);

    if (p) {
      ws::concurrency::SpinMutex::ScopedLock lock(page_mutex_);
      PaddedPage* q = tail_page_.load(std::memory_order_relaxed);
      if (IsValidPage(q)) {
        q->next = p;
      } else {
        head_page_.store(p, std::memory_order_relaxed);
      }
      tail_page_.store(p, std::memory_order_relaxed);
    } else {
      p = tail_page_.load(std::memory_order_relaxed);
    }
    return index;
  }

  template <typename... TArgs>
  void Push(ticket_type k, queue_rep_type& base,
            queue_allocator_type& allocator, TArgs&&... args) {
    PaddedPage* p = nullptr;
    page_allocator_type page_allocator(allocator);
    size_type index = PreparePage(k, base, page_allocator, p);
    assert(p != nullptr && "Page was not prepared");

    auto value_guard = ws::concurrency::detail::templates::MakeRaiiGuard([&] {
      ++base.n_invalid_entries_;
      tail_counter_.fetch_add(queue_rep_type::kNQueue);
    });

    page_allocator_traits::construct(page_allocator, &(*p)[index],
                                     std::forward<TArgs>(args)...);

    p->mask.store(p->mask.load(std::memory_order_relaxed) | uintptr_t(1)
                                                                << index,
                  std::memory_order_relaxed);

    value_guard.Dismiss();
    tail_counter_.fetch_add(queue_rep_type::kNQueue);
  }

  void AbortPush(ticket_type k, queue_rep_type& base,
                 queue_allocator_type& allocator) {
    PaddedPage* p = nullptr;
    PreparePage(k, base, allocator, p);
    ++base.n_invalid_entries_;
    tail_counter_.fetch_add(queue_rep_type::kNQueue);
  }

  bool Pop(void* dst, ticket_type k, queue_rep_type& base,
           queue_allocator_type& allocator) {
    k &= -queue_rep_type::kNQueue;
    ws::concurrency::SpinWaitUntilEq(head_counter_, k);
    ws::concurrency::SpinWaitWhileEq(tail_counter_, k);
    PaddedPage* p = head_page_.load(std::memory_order_relaxed);
    assert(p);
    size_type index = ws::arch::detail::ModulusPowerOfTwo(
        k / queue_rep_type::kNQueue, kItemsPerPage);
    bool success = false;
    {
      page_allocator_type page_allocator(allocator);
      MicroQueuePopFinalizer<self_type, value_type, page_allocator_type>
          finalizer(*this, page_allocator, k + queue_rep_type::kNQueue,
                    index == kItemsPerPage - 1 ? p : nullptr);
      if (p->mask.load(std::memory_order_relaxed) &
          (std::uintptr_t(1) << index)) {
        success = true;
        AssignAndDestroyItem(dst, *p, index);
      } else {
        --base.n_invalid_entries_;
      }
    }
    return success;
  }

  MicroQueue& Assign(const MicroQueue& src, queue_allocator_type& allocator,
                     item_constructor_type construct_item) {
    head_counter_.store(src.head_counter_.load(std::memory_order_relaxed),
                        std::memory_order_relaxed);
    tail_counter_.store(src.tail_counter_.load(std::memory_order_relaxed),
                        std::memory_order_relaxed);

    const PaddedPage* kSrcp = src.head_page_.load(std::memory_order_relaxed);
    if (IsValidPage(kSrcp)) {
      ticket_type g_index = head_counter_.load(std::memory_order_relaxed);
      size_type n_items = (tail_counter_.load(std::memory_order_relaxed) -
                           head_counter_.load(std::memory_order_relaxed)) /
                          queue_rep_type::kNQueue;
      size_type index = ws::arch::detail::ModulusPowerOfTwo(
          head_counter_.load(std::memory_order_relaxed) /
              queue_rep_type::kNQueue,
          kItemsPerPage);
      size_type end_in_first_page =
          (index + n_items < kItemsPerPage) ? (index + n_items) : kItemsPerPage;

      ws::concurrency::detail::templates::TryCall([&] {
        head_page_.store(MakeCopy(allocator, kSrcp, index, end_in_first_page,
                                  g_index, construct_item),
                         std::memory_order_relaxed);
      }).OnException([&] {
        head_counter_.store(0, std::memory_order_relaxed);
        tail_counter_.store(0, std::memory_order_relaxed);
      });
      PaddedPage* cur_page = head_page_.load(std::memory_order_relaxed);

      ws::concurrency::detail::templates::TryCall([&] {
        if (kSrcp != src.tail_page_.load(std::memory_order_relaxed)) {
          for (kSrcp = kSrcp->next;
               kSrcp != src.tail_page_.load(std::memory_order_relaxed);
               kSrcp = kSrcp->next) {
            cur_page->next = MakeCopy(allocator, kSrcp, 0, kItemsPerPage,
                                      g_index, construct_item);
            cur_page = cur_page->next;
          }

          assert(kSrcp == src.tail_page_.load(std::memory_order_relaxed));
          size_type last_index = ws::arch::detail::ModulusPowerOfTwo(
              tail_counter_.load(std::memory_order_relaxed) /
                  queue_rep_type::kNQueue,
              kItemsPerPage);
          if (last_index == 0) last_index = kItemsPerPage;

          cur_page->next = MakeCopy(allocator, kSrcp, 0, last_index, g_index,
                                    construct_item);
          cur_page = cur_page->next;
        }
        tail_page_.store(cur_page, std::memory_order_relaxed);
      }).OnException([&] {
        PaddedPage* invalid_page =
            reinterpret_cast<PaddedPage*>(std::uintptr_t(1));
        tail_page_.store(invalid_page, std::memory_order_relaxed);
      });
    } else {
      head_page_.store(nullptr, std::memory_order_relaxed);
      tail_page_.store(nullptr, std::memory_order_relaxed);
    }
    return *this;
  }

  PaddedPage* MakeCopy(queue_allocator_type& allocator,
                       const PaddedPage* src_page, size_type begin_in_page,
                       size_type end_in_page, ticket_type& g_index,
                       item_constructor_type construct_item) {
    page_allocator_type page_allocator(allocator);
    PaddedPage* new_page = page_allocator_traits::allocate(page_allocator, 1);
    new_page->next = nullptr;
    new_page->mask.store(src_page->mask.load(std::memory_order_relaxed),
                         std::memory_order_relaxed);
    for (; begin_in_page != end_in_page; ++begin_in_page, ++g_index) {
      if (new_page->mask.load(std::memory_order_relaxed) &
          uintptr_t(1) << begin_in_page) {
        CopyItem(*new_page, begin_in_page, *src_page, begin_in_page,
                 construct_item);
      }
    }
    return new_page;
  }

  void InvalidatePage(ticket_type k) {
    PaddedPage* invalid_page = reinterpret_cast<PaddedPage*>(std::uintptr_t(1));
    {
      ws::concurrency::SpinMutex::ScopedLock lock(page_mutex_);
      tail_counter_.store(k + queue_rep_type::kNQueue + 1,
                          std::memory_order_relaxed);
      PaddedPage* q = tail_page_.load(std::memory_order_relaxed);
      if (IsValidPage(q)) {
        q->next = invalid_page;
      } else {
        head_page_.store(invalid_page, std::memory_order_relaxed);
      }
      tail_page_.store(invalid_page, std::memory_order_relaxed);
    }
  }

  PaddedPage* HeadPage() { return head_page_.load(std::memory_order_relaxed); }

  void Clear(queue_allocator_type& allocator, PaddedPage* new_head = nullptr,
             PaddedPage* new_tail = nullptr) {
    PaddedPage* curr_page = HeadPage();
    size_type index = (head_counter_.load(std::memory_order_relaxed) /
                       queue_rep_type::kNQueue) %
                      kItemsPerPage;
    page_allocator_type page_allocator(allocator);

    while (curr_page && IsValidPage(curr_page)) {
      while (index != kItemsPerPage) {
        if (curr_page->mask.load(std::memory_order_relaxed) &
            (std::uintptr_t(1) << index)) {
          page_allocator_traits::destroy(page_allocator,
                                         &curr_page->operator[](index));
        }
        ++index;
      }

      index = 0;
      PaddedPage* next_page = curr_page->next;
      page_allocator_traits::destroy(page_allocator, curr_page);
      page_allocator_traits::deallocate(page_allocator, curr_page, 1);
      curr_page = next_page;
    }
    head_counter_.store(0, std::memory_order_relaxed);
    tail_counter_.store(0, std::memory_order_relaxed);
    head_page_.store(new_head, std::memory_order_relaxed);
    tail_page_.store(new_tail, std::memory_order_relaxed);
  }

  void ClearAndInvalidate(queue_allocator_type& allocator) {
    PaddedPage* invalid_page = reinterpret_cast<PaddedPage*>(std::uintptr_t(1));
    clear(allocator, invalid_page, invalid_page);
  }

 protected:
  using page_allocator_traits = std::allocator_traits<page_allocator_type>;

 private:
  friend class MicroQueuePopFinalizer<self_type, value_type,
                                      page_allocator_type>;

  class Destroyer {
    value_type& value_;

   public:
    Destroyer(reference value) : value_(value) {}
    Destroyer(const Destroyer&) = delete;
    Destroyer& operator=(const Destroyer&) = delete;
    ~Destroyer() { value_.~T(); }
  };

  void CopyItem(PaddedPage& dst, size_type dindex, const PaddedPage& src,
                size_type sindex, item_constructor_type construct_item) {
    auto& src_item = src[sindex];
    construct_item(&dst[dindex], static_cast<const void*>(&src_item));
  }

  void AssignAndDestroyItem(void* dst, PaddedPage& src, size_type index) {
    auto& from = src[index];
    Destroyer d(from);
    *static_cast<T*>(dst) = std::move(from);
  }

  void SpinWaitUntilTurn(std::atomic<ticket_type>& counter, ticket_type k,
                         queue_rep_type& rb) const {
    for (ws::concurrency::detail::AtomicBackoff b{};; b.Wait()) {
      ticket_type c = counter.load(std::memory_order_acquire);
      if (c == k)
        return;
      else if (c & 1) {
        ++rb.n_invalid_entries_;
        throw std::bad_alloc();
      }
    }
  }

  std::atomic<PaddedPage*> head_page_{};
  std::atomic<ticket_type> head_counter_{};

  std::atomic<PaddedPage*> tail_page_{};
  std::atomic<ticket_type> tail_counter_{};

  ws::concurrency::SpinMutex page_mutex_{};
};

#if _MSC_VER && !defined(__INTEL_COMPILER)
#pragma warning(pop)
#endif

template <typename TContainer, typename T, typename TAllocator>
class MicroQueuePopFinalizer {
 public:
  using PaddedPage = typename TContainer::PaddedPage;
  using allocator_type = TAllocator;
  using allocator_traits_type = std::allocator_traits<allocator_type>;

  MicroQueuePopFinalizer(TContainer& queue, TAllocator& alloc, ticket_type k,
                         PaddedPage* p)
      : ticket_(k), queue_(queue), page_(p), allocator_(alloc) {}

  MicroQueuePopFinalizer(const MicroQueuePopFinalizer&) = delete;
  MicroQueuePopFinalizer& operator=(const MicroQueuePopFinalizer&) = delete;

  ~MicroQueuePopFinalizer() {
    PaddedPage* p = page_;
    if (IsValidPage(p)) {
      ws::concurrency::SpinMutex::ScopedLock lock(queue_.page_mutex_);
      PaddedPage* q = p->next;
      queue_.head_page_.store(q, std::memory_order_relaxed);
      if (!IsValidPage(q)) {
        queue_.tail_page_.store(nullptr, std::memory_order_relaxed);
      }
    }
    queue_.head_counter_.store(ticket_, std::memory_order_release);
    if (IsValidPage(p)) {
      allocator_traits_type::destroy(allocator_, static_cast<PaddedPage*>(p));
      allocator_traits_type::deallocate(allocator_, static_cast<PaddedPage*>(p),
                                        1);
    }
  }

 private:
  ticket_type ticket_;
  TContainer& queue_;
  PaddedPage* page_;
  TAllocator& allocator_;
};

#if _MSC_VER && !defined(__INTEL_COMPILER)
#pragma warning(push)
#pragma warning(disable : 4324)
#endif

template <typename T, typename TAllocator>
struct ConcurrentQueueRep {
  using self_type = ConcurrentQueueRep<T, TAllocator>;
  using size_type = std::size_t;
  using MicroQueue_type = MicroQueue<T, TAllocator>;
  using allocator_type = TAllocator;
  using allocator_traits_type = std::allocator_traits<allocator_type>;
  using PaddedPage = typename MicroQueue_type::PaddedPage;
  using page_allocator_type = typename MicroQueue_type::page_allocator_type;
  using item_constructor_type = typename MicroQueue_type::item_constructor_type;

 private:
  using page_allocator_traits = std::allocator_traits<page_allocator_type>;
  using queue_allocator_type =
      typename allocator_traits_type::template rebind_alloc<self_type>;

 public:
  static constexpr size_type kNQueue = 8;

  static constexpr size_type kPhi = 3;
  static constexpr size_type kItemSize = MicroQueue_type::kItemSize;
  static constexpr size_type kItemsPerPage = MicroQueue_type::kItemsPerPage;

  ConcurrentQueueRep() {}

  ConcurrentQueueRep(const ConcurrentQueueRep&) = delete;
  ConcurrentQueueRep& operator=(const ConcurrentQueueRep&) = delete;

  void Clear(queue_allocator_type& alloc) {
    for (size_type index = 0; index < kNQueue; ++index) {
      array_[index].Clear(alloc);
    }
    head_counter_.store(0, std::memory_order_relaxed);
    tail_counter_.store(0, std::memory_order_relaxed);
    n_invalid_entries_.store(0, std::memory_order_relaxed);
  }

  void Assign(const ConcurrentQueueRep& src, queue_allocator_type& alloc,
              item_constructor_type construct_item) {
    head_counter_.store(src.head_counter_.load(std::memory_order_relaxed),
                        std::memory_order_relaxed);
    tail_counter_.store(src.tail_counter_.load(std::memory_order_relaxed),
                        std::memory_order_relaxed);
    n_invalid_entries_.store(
        src.n_invalid_entries_.load(std::memory_order_relaxed),
        std::memory_order_relaxed);

    size_type queue_idx = 0;
    ws::concurrency::detail::templates::TryCall([&] {
      for (; queue_idx < kNQueue; ++queue_idx) {
        array_[queue_idx].assign(src.array_[queue_idx], alloc, construct_item);
      }
    }).OnException([&] {
      for (size_type i = 0; i < queue_idx + 1; ++i) {
        array_[i].ClearAndInvalidate(alloc);
      }
      head_counter_.store(0, std::memory_order_relaxed);
      tail_counter_.store(0, std::memory_order_relaxed);
      n_invalid_entries_.store(0, std::memory_order_relaxed);
    });

    assert(head_counter_.load(std::memory_order_relaxed) ==
               src.head_counter_.load(std::memory_order_relaxed) &&
           tail_counter_.load(std::memory_order_relaxed) ==
               src.tail_counter_.load(std::memory_order_relaxed) &&
           "the source concurrent queue should not be concurrently modified.");
  }

  bool Empty() const {
    ticket_type tc = tail_counter_.load(std::memory_order_acquire);
    ticket_type hc = head_counter_.load(std::memory_order_relaxed);

    return tc == tail_counter_.load(std::memory_order_relaxed) &&
           std::ptrdiff_t(tc - hc -
                          n_invalid_entries_.load(std::memory_order_relaxed)) <=
               0;
  }

  std::ptrdiff_t Size() const {
    assert(sizeof(std::ptrdiff_t) <= sizeof(size_type));
    std::ptrdiff_t hc = head_counter_.load(std::memory_order_acquire);
    std::ptrdiff_t tc = tail_counter_.load(std::memory_order_relaxed);
    std::ptrdiff_t nie = n_invalid_entries_.load(std::memory_order_relaxed);

    return tc - hc - nie;
  }

  friend class MicroQueue<T, TAllocator>;

  static size_type Index(ticket_type k) { return k * kPhi % kNQueue; }

  MicroQueue_type& Choose(ticket_type k) { return array_[Index(k)]; }

  alignas(ws::arch::detail::CacheLineSize()) MicroQueue_type array_[kNQueue];

  alignas(ws::arch::detail::CacheLineSize())
      std::atomic<ticket_type> head_counter_{};
  alignas(ws::arch::detail::CacheLineSize())
      std::atomic<ticket_type> tail_counter_{};
  alignas(ws::arch::detail::CacheLineSize())
      std::atomic<size_type> n_invalid_entries_{};
};

#if _MSC_VER && !defined(__INTEL_COMPILER)
#pragma warning(pop)
#endif

template <typename TQueueRep, typename TAllocator>
inline std::pair<bool, ticket_type> InternalTryPopImpl(void* dst,
                                                       TQueueRep& queue,
                                                       TAllocator& alloc) {
  ticket_type ticket{};
  do {
    ticket = queue.head_counter_.load(std::memory_order_acquire);
    do {
      if (static_cast<std::ptrdiff_t>(
              queue.tail_counter_.load(std::memory_order_relaxed) - ticket) <=
          0) {
        return {false, ticket};
      }

    } while (!queue.head_counter_.compare_exchange_strong(ticket, ticket + 1));
  } while (!queue.Choose(ticket).Pop(dst, ticket, queue, alloc));
  return {true, ticket};
}

static constexpr std::size_t kCbqSlotsAvailTag = 0;
static constexpr std::size_t kCbqItemsAvailTag = 1;
static constexpr std::size_t kMonitorsNumber = 2;

inline std::uint8_t* AllocateBoundedQueueRep(std::size_t queue_rep_size) {
  std::size_t monitors_mem_size =
      sizeof(ws::concurrency::detail::ConcurrentMonitor) * kMonitorsNumber;
  std::uint8_t* mem =
      static_cast<std::uint8_t*>(ws::concurrency::detail::CacheAlignedAllocate(
          queue_rep_size + monitors_mem_size));

  ws::concurrency::detail::ConcurrentMonitor* monitors =
      reinterpret_cast<ws::concurrency::detail::ConcurrentMonitor*>(
          mem + queue_rep_size);
  for (std::size_t i = 0; i < kMonitorsNumber; ++i) {
    new (monitors + i) ws::concurrency::detail::ConcurrentMonitor();
  }

  return mem;
}

inline void DeallocateBoundedQueueRep(std::uint8_t* mem,
                                      std::size_t queue_rep_size) {
  ws::concurrency::detail::ConcurrentMonitor* monitors =
      reinterpret_cast<ws::concurrency::detail::ConcurrentMonitor*>(
          mem + queue_rep_size);
  for (std::size_t i = 0; i < kMonitorsNumber; ++i) {
    monitors[i].~ConcurrentMonitor();
  }

  ws::concurrency::detail::CacheAlignedDeallocate(mem);
}

inline void WaitBoundedQueueMonitor(
    ws::concurrency::detail::ConcurrentMonitor* monitors,
    std::size_t monitor_tag, std::ptrdiff_t target,
    const ws::Delegate<bool()>& predicate) {
  assert(monitor_tag < kMonitorsNumber);
  ws::concurrency::detail::ConcurrentMonitor& monitor = monitors[monitor_tag];

  monitor.Wait<ws::concurrency::detail::ConcurrentMonitor::thread_context>(
      [&] { return !predicate(); }, std::uintptr_t(target));
}

inline void AbortBoundedQueueMonitors(
    ws::concurrency::detail::ConcurrentMonitor* monitors) {
  ws::concurrency::detail::ConcurrentMonitor& items_avail =
      monitors[kCbqItemsAvailTag];
  ws::concurrency::detail::ConcurrentMonitor& slots_avail =
      monitors[kCbqSlotsAvailTag];

  items_avail.AbortAll();
  slots_avail.AbortAll();
}

struct predicate_leq {
  std::size_t ticket_;
  predicate_leq(std::size_t ticket) : ticket_(ticket) {}
  bool operator()(std::uintptr_t ticket) const {
    return static_cast<std::size_t>(ticket) <= ticket_;
  }
};

void inline NotifyBoundedQueueMonitor(
    ws::concurrency::detail::ConcurrentMonitor* monitors,
    std::size_t monitor_tag, std::size_t ticket) {
  assert(monitor_tag < kMonitorsNumber);
  ws::concurrency::detail::ConcurrentMonitor& monitor = monitors[monitor_tag];
  monitor.Notify(predicate_leq(ticket));
}

}  // namespace detail
}  // namespace concurrency
}  // namespace ws
