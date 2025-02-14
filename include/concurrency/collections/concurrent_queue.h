#pragma once

#include <utility>

#include "concurrency/aligned_allocator.h"
#include "concurrency/detail/concurrent_queue_base.h"

namespace ws {
namespace concurrency {
namespace collections {
template <typename QueueRep, typename Allocator>
inline std::pair<bool, ws::concurrency::detail::ticket_type> InternalTryPopImpl(
    void* dst, QueueRep& queue, Allocator& alloc) {
  ws::concurrency::detail::ticket_type ticket{};
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

template <typename T, typename Allocator = ws::concurrency::AlignedAllocator<T>>
class ConcurrentQueue {
  using allocator_traits_type = ws::concurrency::AllocatorTraits<Allocator>;
  using queue_representation_type =
      ws::concurrency::detail::ConcurrentQueueRep<T, Allocator>;
  using queue_allocator_type =
      typename allocator_traits_type::template rebind_alloc<
          queue_representation_type>;
  using queue_allocator_traits =
      ws::concurrency::AllocatorTraits<queue_allocator_type>;

 public:
  using size_type = std::size_t;
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using difference_type = std::ptrdiff_t;

  using allocator_type = Allocator;
  using pointer = typename allocator_traits_type::pointer;
  using const_pointer = typename allocator_traits_type::const_pointer;

  ConcurrentQueue() : ConcurrentQueue(allocator_type()) {}

  explicit ConcurrentQueue(const allocator_type& a)
      : allocator_(a), queue_rep_ptr(nullptr) {
    queue_rep_ptr = static_cast<queue_representation_type*>(
        ws::concurrency::CacheAlignedAllocate(
            sizeof(queue_representation_type)));
    queue_allocator_traits::construct(allocator_, queue_rep_ptr);
    assert(ws::concurrency::detail::IsAligned(
               queue_rep_ptr, ws::concurrency::detail::CacheLineSize()) &&
           "alignment error");
    assert(ws::concurrency::detail::IsAligned(
               &queue_rep_ptr->head_counter_,
               ws::concurrency::detail::CacheLineSize()) &&
           "alignment error");
    assert(ws::concurrency::detail::IsAligned(
               &queue_rep_ptr->tail_counter_,
               ws::concurrency::detail::CacheLineSize()) &&
           "alignment error");
    assert(
        ws::concurrency::detail::IsAligned(
            &queue_rep_ptr->array_, ws::concurrency::detail::CacheLineSize()) &&
        "alignment error");
  }

  template <typename InputIterator>
  ConcurrentQueue(InputIterator begin, InputIterator end,
                  const allocator_type& a = allocator_type())
      : ConcurrentQueue(a) {
    for (; begin != end; ++begin) push(*begin);
  }

  ConcurrentQueue(std::initializer_list<value_type> init,
                  const allocator_type& alloc = allocator_type())
      : ConcurrentQueue(init.begin(), init.end(), alloc) {}

  ConcurrentQueue(const ConcurrentQueue& src, const allocator_type& a)
      : ConcurrentQueue(a) {
    queue_rep_ptr->assign(*src.queue_rep_ptr, allocator_, CopyConstructItem);
  }

  ConcurrentQueue(const ConcurrentQueue& src)
      : ConcurrentQueue(
            queue_allocator_traits::select_on_container_copy_construction(
                src.Allocator())) {
    queue_rep_ptr->assign(*src.queue_rep_ptr, allocator_, CopyConstructItem);
  }

  ConcurrentQueue(ConcurrentQueue&& src)
      : ConcurrentQueue(std::move(src.allocator_)) {
    internal_swap(src);
  }

  ConcurrentQueue(ConcurrentQueue&& src, const allocator_type& a)
      : ConcurrentQueue(a) {
    if (allocator_ == src.allocator_) {
      internal_swap(src);
    } else {
      queue_rep_ptr->assign(*src.queue_rep_ptr, allocator_, MoveConstructItem);
      src.clear();
    }
  }

  ~ConcurrentQueue() {
    Clear();
    queue_rep_ptr->Clear(allocator_);
    queue_allocator_traits::destroy(allocator_, queue_rep_ptr);
    ws::concurrency::CacheAlignedDeallocate(queue_rep_ptr);
  }

  ConcurrentQueue& operator=(const ConcurrentQueue& other) {
    if (queue_rep_ptr != other.queue_rep_ptr) {
      Clear();
      allocator_ = other.allocator_;
      queue_rep_ptr->assign(*other.queue_rep_ptr, allocator_,
                            CopyConstructItem);
    }
    return *this;
  }

  ConcurrentQueue& operator=(ConcurrentQueue&& other) {
    if (queue_rep_ptr != other.queue_rep_ptr) {
      Clear();
      if (allocator_ == other.allocator_) {
        internal_swap(other);
      } else {
        queue_rep_ptr->assign(*other.queue_rep_ptr, other.allocator_,
                              MoveConstructItem);
        other.clear();
        allocator_ = std::move(other.allocator_);
      }
    }
    return *this;
  }

  ConcurrentQueue& operator=(std::initializer_list<value_type> init) {
    assign(init);
    return *this;
  }

  template <typename InputIterator>
  void Assign(InputIterator first, InputIterator last) {
    ConcurrentQueue src(first, last);
    Clear();
    queue_rep_ptr->assign(*src.queue_rep_ptr, allocator_, MoveConstructItem);
  }

  void Assign(std::initializer_list<value_type> init) {
    assign(init.begin(), init.end());
  }

  void Swap(ConcurrentQueue& other) {
    assert(allocator_ == other.allocator_ && "unequal allocators");
    internal_swap(other);
  }

  void Push(const T& value) { InternalPush(value); }

  void Push(T&& value) { InternalPush(std::move(value)); }

  template <typename... Args>
  void Emplace(Args&&... args) {
    InternalPush(std::forward<Args>(args)...);
  }

  bool TryPop(T& result) { return InternalTryPop(&result); }

  size_type UnsafeSize() const {
    std::ptrdiff_t size = queue_rep_ptr->size();
    return size < 0 ? 0 : size_type(size);
  }

  [[nodiscard]] bool Empty() const { return queue_rep_ptr->Empty(); }

  void Clear() { queue_rep_ptr->Clear(allocator_); }

  allocator_type GetAllocator() const { return allocator_; }

 private:
  void InternalSwap(ConcurrentQueue& src) {
    using std::swap;
    swap(queue_rep_ptr, src.queue_rep_ptr);
  }

  template <typename... Args>
  void InternalPush(Args&&... args) {
    ws::concurrency::detail::ticket_type k = queue_rep_ptr->tail_counter_++;
    queue_rep_ptr->Choose(k).Push(k, *queue_rep_ptr, allocator_,
                                  std::forward<Args>(args)...);
  }

  bool InternalTryPop(void* dst) {
    return InternalTryPopImpl(dst, *queue_rep_ptr, allocator_).first;
  }

  static void CopyConstructItem(T* location, const void* src) {
    new (location) value_type(*static_cast<const value_type*>(src));
  }

  static void MoveConstructItem(T* location, const void* src) {
    new (location) value_type(
        std::move(*static_cast<value_type*>(const_cast<void*>(src))));
  }

  queue_allocator_type allocator_;
  queue_representation_type* queue_rep_ptr;

  friend void Swap(ConcurrentQueue& lhs, ConcurrentQueue& rhs) {
    lhs.swap(rhs);
  }

  friend bool operator==(const ConcurrentQueue& lhs,
                         const ConcurrentQueue& rhs) {
    return lhs.unsafe_size() == rhs.unsafe_size() &&
           std::equal(lhs.unsafe_begin(), lhs.unsafe_end(), rhs.unsafe_begin());
  }

  friend bool operator!=(const ConcurrentQueue& lhs,
                         const ConcurrentQueue& rhs) {
    return !(lhs == rhs);
  }
};
}  // namespace collections
}  // namespace concurrency
}  // namespace ws
