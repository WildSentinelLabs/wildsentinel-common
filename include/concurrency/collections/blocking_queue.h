
#pragma once
#include <cstdint>
#include <memory>

#include "concurrency/detail/concurrent_monitor.h"
#include "concurrency/detail/concurrent_queue_base.h"
#include "delegate.h"

namespace ws {
namespace concurrency {
namespace collections {

template <typename T, typename Allocator = std::allocator<T>>
class BlockingQueue {
  using allocator_traits_type = std::allocator_traits<Allocator>;
  using queue_representation_type =
      ws::concurrency::detail::ConcurrentQueueRep<T, Allocator>;
  using queue_allocator_type =
      typename allocator_traits_type::template rebind_alloc<
          queue_representation_type>;
  using queue_allocator_traits = std::allocator_traits<queue_allocator_type>;

  void InternalWait(ws::concurrency::detail::ConcurrentMonitor* monitors,
                    std::size_t monitor_tag, std::ptrdiff_t target,
                    ws::Delegate<bool()> predicate) {
    ws::concurrency::detail::WaitBoundedQueueMonitor(monitors, monitor_tag,
                                                     target, predicate);
  }

 public:
  using size_type = std::ptrdiff_t;
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using difference_type = std::ptrdiff_t;

  using allocator_type = Allocator;
  using pointer = typename allocator_traits_type::pointer;
  using const_pointer = typename allocator_traits_type::const_pointer;

  BlockingQueue() : BlockingQueue(allocator_type()) {}

  explicit BlockingQueue(const allocator_type& a)
      : allocator_(a),
        capacity_(0),
        abort_counter_(0),
        queue_rep_ptr_(nullptr) {
    queue_rep_ptr_ = reinterpret_cast<queue_representation_type*>(
        ws::concurrency::detail::AllocateBoundedQueueRep(
            sizeof(queue_representation_type)));
    monitors_ = reinterpret_cast<ws::concurrency::detail::ConcurrentMonitor*>(
        queue_rep_ptr_ + 1);
    queue_allocator_traits::construct(allocator_, queue_rep_ptr_);
    capacity_ = std::size_t(-1) / (queue_representation_type::kItemSize > 1
                                       ? queue_representation_type::kItemSize
                                       : 2);

    assert(ws::arch::detail::IsAligned(queue_rep_ptr_,
                                       ws::arch::detail::CacheLineSize()) &&
           "alignment error");
    assert(ws::arch::detail::IsAligned(&queue_rep_ptr_->head_counter_,
                                       ws::arch::detail::CacheLineSize()) &&
           "alignment error");
    assert(ws::arch::detail::IsAligned(&queue_rep_ptr_->tail_counter_,
                                       ws::arch::detail::CacheLineSize()) &&
           "alignment error");
    assert(ws::arch::detail::IsAligned(&queue_rep_ptr_->array_,
                                       ws::arch::detail::CacheLineSize()) &&
           "alignment error");
  }

  BlockingQueue(const BlockingQueue& src, const allocator_type& a)
      : BlockingQueue(a) {
    capacity_ = src.capacity_;
    queue_rep_ptr_->Assign(*src.queue_rep_ptr_, allocator_, CopyConstructItem);
  }

  BlockingQueue(const BlockingQueue& src)
      : BlockingQueue(
            queue_allocator_traits::select_on_container_copy_construction(
                src.get_allocator())) {
    capacity_ = src.capacity_;
    queue_rep_ptr_->Assign(*src.queue_rep_ptr_, allocator_, CopyConstructItem);
  }

  BlockingQueue(BlockingQueue&& src)
      : BlockingQueue(std::move(src.allocator_)) {
    InternalSwap(src);
  }

  BlockingQueue(BlockingQueue&& src, const allocator_type& a)
      : BlockingQueue(a) {
    if (allocator_ == src.allocator_) {
      InternalSwap(src);
    } else {
      queue_rep_ptr_->Assign(*src.queue_rep_ptr_, allocator_,
                             MoveConstructItem);
      src.Clear();
    }
  }

  ~BlockingQueue() {
    Clear();
    queue_allocator_traits::destroy(allocator_, queue_rep_ptr_);
    ws::concurrency::detail::DeallocateBoundedQueueRep(
        reinterpret_cast<std::uint8_t*>(queue_rep_ptr_),
        sizeof(queue_representation_type));
  }

  BlockingQueue& operator=(const BlockingQueue& other) {
    if (queue_rep_ptr_ != other.queue_rep_ptr_) {
      Clear();
      allocator_ = other.allocator_;
      capacity_ = other.capacity_;
      queue_rep_ptr_->Assign(*other.queue_rep_ptr_, allocator_,
                             CopyConstructItem);
    }
    return *this;
  }

  BlockingQueue& operator=(BlockingQueue&& other) {
    if (queue_rep_ptr_ != other.queue_rep_ptr_) {
      Clear();
      if (allocator_ == other.allocator_) {
        InternalSwap(other);
      } else {
        queue_rep_ptr_->Assign(*other.queue_rep_ptr_, other.allocator_,
                               MoveConstructItem);
        other.clear();
        allocator_ = std::move(other.allocator_);
        capacity_ = other.capacity_;
      }
    }
    return *this;
  }

  void Swap(BlockingQueue& other) {
    assert(allocator_ == other.allocator_ && "unequal allocators");
    InternalSwap(other);
  }

  void Push(const T& value) { InternalPush(value); }

  void Push(T&& value) { InternalPush(std::move(value)); }

  bool TryPush(const T& value) { return InternalPushIfNotFull(value); }

  bool TryPush(T&& value) { return InternalPushIfNotFull(std::move(value)); }

  template <typename... TArgs>
  void Emplace(TArgs&&... args) {
    InternalPush(std::forward<TArgs>(args)...);
  }

  template <typename... TArgs>
  bool TryEmplace(TArgs&&... args) {
    return InternalPushIfNotFull(std::forward<TArgs>(args)...);
  }

  void Pop(T& result) { InternalPop(&result); }

  /** Does not wait for item to become available.
      Returns true if successful; false otherwise. */
  bool TryPop(T& result) { return InternalPopIfPresent(&result); }

  void Abort() { InternalAbort(); }

  std::ptrdiff_t Size() const { return queue_rep_ptr_->Size(); }

  void SetCapacity(size_type new_capacity) {
    std::ptrdiff_t c = new_capacity < 0 ? kInfiniteCapacity : new_capacity;
    capacity_ = c;
  }

  size_type Capacity() const { return capacity_; }

  _GLIBCXX_NODISCARD bool Empty() const { return queue_rep_ptr_->Empty(); }

  void Clear() { queue_rep_ptr_->Clear(allocator_); }

  allocator_type get_allocator() const { return allocator_; }

 private:
  void InternalSwap(BlockingQueue& src) {
    std::swap(queue_rep_ptr_, src.queue_rep_ptr_);
    std::swap(capacity_, src.capacity_);
    std::swap(monitors_, src.monitors_);
  }

  static constexpr std::ptrdiff_t kInfiniteCapacity =
      std::ptrdiff_t(~size_type(0) / 2);

  template <typename... TArgs>
  void InternalPush(TArgs&&... args) {
    unsigned old_abort_counter = abort_counter_.load(std::memory_order_relaxed);
    ws::concurrency::detail::ticket_type ticket =
        queue_rep_ptr_->tail_counter_++;
    std::ptrdiff_t target = ticket - capacity_;

    if (static_cast<std::ptrdiff_t>(queue_rep_ptr_->head_counter_.load(
            std::memory_order_relaxed)) <= target) {
      Delegate<bool()> pred = [&] {
        if (abort_counter_.load(std::memory_order_relaxed) !=
            old_abort_counter) {
          throw std::runtime_error("user_abort");
        }

        return static_cast<std::ptrdiff_t>(queue_rep_ptr_->head_counter_.load(
                   std::memory_order_relaxed)) <= target;
      };

      ws::concurrency::detail::templates::TryCall([&] {
        InternalWait(monitors_, ws::concurrency::detail::kCbqSlotsAvailTag,
                     target, pred);
      }).OnException([&] {
        queue_rep_ptr_->Choose(ticket).AbortPush(ticket, *queue_rep_ptr_,
                                                 allocator_);
      });
    }
    assert(static_cast<std::ptrdiff_t>(queue_rep_ptr_->head_counter_.load(
               std::memory_order_relaxed)) > target);
    queue_rep_ptr_->Choose(ticket).Push(ticket, *queue_rep_ptr_, allocator_,
                                        std::forward<TArgs>(args)...);
    ws::concurrency::detail::NotifyBoundedQueueMonitor(
        monitors_, ws::concurrency::detail::kCbqItemsAvailTag, ticket);
  }

  template <typename... TArgs>
  bool InternalPushIfNotFull(TArgs&&... args) {
    ws::concurrency::detail::ticket_type ticket =
        queue_rep_ptr_->tail_counter_.load(std::memory_order_relaxed);
    do {
      if (static_cast<std::ptrdiff_t>(
              ticket - queue_rep_ptr_->head_counter_.load(
                           std::memory_order_relaxed)) >= capacity_) {
        return false;
      }

    } while (!queue_rep_ptr_->tail_counter_.compare_exchange_strong(
        ticket, ticket + 1));

    queue_rep_ptr_->Choose(ticket).Push(ticket, *queue_rep_ptr_, allocator_,
                                        std::forward<TArgs>(args)...);
    ws::concurrency::detail::NotifyBoundedQueueMonitor(
        monitors_, ws::concurrency::detail::kCbqItemsAvailTag, ticket);
    return true;
  }

  void InternalPop(void* dst) {
    std::ptrdiff_t target;

    unsigned old_abort_counter = abort_counter_.load(std::memory_order_relaxed);

    do {
      target = queue_rep_ptr_->head_counter_++;
      if (static_cast<std::ptrdiff_t>(queue_rep_ptr_->tail_counter_.load(
              std::memory_order_relaxed)) <= target) {
        auto pred = [&] {
          if (abort_counter_.load(std::memory_order_relaxed) !=
              old_abort_counter) {
            throw std::runtime_error("user_abort");
          }

          return static_cast<std::ptrdiff_t>(queue_rep_ptr_->tail_counter_.load(
                     std::memory_order_relaxed)) <= target;
        };

        ws::concurrency::detail::templates::TryCall([&] {
          InternalWait(monitors_, ws::concurrency::detail::kCbqItemsAvailTag,
                       target, pred);
        }).OnException([&] { queue_rep_ptr_->head_counter_--; });
      }
      assert(static_cast<std::ptrdiff_t>(queue_rep_ptr_->tail_counter_.load(
                 std::memory_order_relaxed)) > target);
    } while (!queue_rep_ptr_->Choose(target).Pop(dst, target, *queue_rep_ptr_,
                                                 allocator_));

    ws::concurrency::detail::NotifyBoundedQueueMonitor(
        monitors_, ws::concurrency::detail::kCbqSlotsAvailTag, target);
  }

  bool InternalPopIfPresent(void* dst) {
    bool present{};
    ws::concurrency::detail::ticket_type ticket{};
    std::tie(present, ticket) = ws::concurrency::detail::InternalTryPopImpl(
        dst, *queue_rep_ptr_, allocator_);

    if (present) {
      ws::concurrency::detail::NotifyBoundedQueueMonitor(
          monitors_, ws::concurrency::detail::kCbqSlotsAvailTag, ticket);
    }
    return present;
  }

  void InternalAbort() {
    ++abort_counter_;
    ws::concurrency::detail::AbortBoundedQueueMonitors(monitors_);
  }

  static void CopyConstructItem(T* location, const void* src) {
    new (location) value_type(*static_cast<const value_type*>(src));
  }

  static void MoveConstructItem(T* location, const void* src) {
    new (location) value_type(
        std::move(*static_cast<value_type*>(const_cast<void*>(src))));
  }

  queue_allocator_type allocator_;
  std::ptrdiff_t capacity_;
  std::atomic<unsigned> abort_counter_;
  queue_representation_type* queue_rep_ptr_;

  ws::concurrency::detail::ConcurrentMonitor* monitors_;

  friend void Swap(BlockingQueue& lhs, BlockingQueue& rhs) { lhs.swap(rhs); }

  friend bool operator==(const BlockingQueue& lhs, const BlockingQueue& rhs) {
    return lhs.size() == rhs.size();
  }

#if !_CPP20_COMPARISONS_PRESENT
  friend bool operator!=(const BlockingQueue& lhs, const BlockingQueue& rhs) {
    return !(lhs == rhs);
  }
#endif
};

namespace stl {
template <typename T, typename TAllocator = std::allocator<T>>
class blocking_queue {
 public:
  using queue_type = BlockingQueue<T, TAllocator>;

  using value_type = typename queue_type::value_type;
  using size_type = typename queue_type::size_type;
  using reference = typename queue_type::reference;
  using const_reference = typename queue_type::const_reference;
  using difference_type = typename queue_type::difference_type;
  using allocator_type = typename queue_type::allocator_type;
  using pointer = typename queue_type::pointer;
  using const_pointer = typename queue_type::const_pointer;

 private:
  queue_type internal_instance_;

 public:
  blocking_queue() = default;

  explicit blocking_queue(const allocator_type& alloc)
      : internal_instance_(alloc) {}

  template <typename TInputIt>
  blocking_queue(TInputIt first, TInputIt last,
                 const allocator_type& alloc = allocator_type())
      : internal_instance_(first, last, alloc) {}

  blocking_queue(const blocking_queue& other)
      : internal_instance_(other.internal_instance_) {}

  blocking_queue(blocking_queue&& other) noexcept
      : internal_instance_(std::move(other.internal_instance_)) {}

  blocking_queue(const blocking_queue& other, const allocator_type& alloc)
      : internal_instance_(other.internal_instance_, alloc) {}

  blocking_queue(blocking_queue&& other, const allocator_type& alloc) noexcept
      : internal_instance_(std::move(other.internal_instance_), alloc) {}

  _GLIBCXX_NODISCARD bool empty() const { return internal_instance_.Empty(); }

  _GLIBCXX_NODISCARD
  size_type size() const { return internal_instance_.UnsafeSize(); }

  bool try_push(const value_type& __x) {
    return internal_instance_.TryPush(__x);
  }

  bool try_push(value_type&& __x) {
    return internal_instance_.TryPush(std::move(__x));
  }

  void push(const value_type& __x) { internal_instance_.Push(__x); }

  void push(value_type&& __x) { internal_instance_.Push(std::move(__x)); }

  template <typename... TArgs>
  void emplace(TArgs&&... __args) {
    internal_instance_.Emplace(std::forward<TArgs>(__args)...);
  }

  bool try_pop(value_type& __x) { return internal_instance_.TryPop(__x); }

  void pop(value_type& __x) { internal_instance_.Pop(__x); }

  void pop() {
    value_type & __x;
    internal_instance_.pop(__x);
  }

  void swap(blocking_queue& __q) {
    internal_instance_.Swap(__q.internal_instance_);
  }

  blocking_queue& operator=(const blocking_queue& other) {
    internal_instance_ = other.internal_instance_;
    return *this;
  }

  blocking_queue& operator=(blocking_queue&& other) noexcept {
    internal_instance_ = std::move(other.internal_instance_);
    return *this;
  }
};

template <typename T, typename TAllocator>
void swap(blocking_queue<T, TAllocator>& lhs,
          blocking_queue<T, TAllocator>& rhs) noexcept {
  lhs.swap(rhs);
}

#if _CPP17_DEDUCTION_GUIDES_PRESENT >= 201606
template <typename TContainer, typename = std::allocator<TContainer>>
blocking_queue(TContainer)
    -> blocking_queue<typename TContainer::value_type, TContainer>;
#endif

}  // namespace stl

}  // namespace collections
}  // namespace concurrency
}  // namespace ws
