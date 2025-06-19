#pragma once

#include <utility>

#include "concurrency/detail/concurrent_queue_base.h"

namespace ws {
namespace concurrency {
template <typename T, typename TAllocator = std::allocator<T>>
class ConcurrentQueue {
  using allocator_traits_type = std::allocator_traits<TAllocator>;
  using queue_representation_type =
      ws::concurrency::detail::ConcurrentQueueRep<T, TAllocator>;
  using queue_allocator_type =
      typename allocator_traits_type::template rebind_alloc<
          queue_representation_type>;
  using queue_allocator_traits = std::allocator_traits<queue_allocator_type>;

 public:
  using size_type = std::size_t;
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using difference_type = std::ptrdiff_t;

  using allocator_type = TAllocator;
  using pointer = typename allocator_traits_type::pointer;
  using const_pointer = typename allocator_traits_type::const_pointer;

  ConcurrentQueue() : ConcurrentQueue(allocator_type()) {}

  explicit ConcurrentQueue(const allocator_type& a)
      : allocator_(a), queue_rep_ptr(nullptr) {
    queue_rep_ptr = static_cast<queue_representation_type*>(
        ws::concurrency::detail::CacheAlignedAllocate(
            sizeof(queue_representation_type)));
    queue_allocator_traits::construct(allocator_, queue_rep_ptr);
    assert(ws::detail::IsAligned(queue_rep_ptr, ws::detail::CacheLineSize()) &&
           "alignment error");
    assert(ws::detail::IsAligned(&queue_rep_ptr->head_counter_,
                                 ws::detail::CacheLineSize()) &&
           "alignment error");
    assert(ws::detail::IsAligned(&queue_rep_ptr->tail_counter_,
                                 ws::detail::CacheLineSize()) &&
           "alignment error");
    assert(ws::detail::IsAligned(&queue_rep_ptr->array_,
                                 ws::detail::CacheLineSize()) &&
           "alignment error");
  }

  ConcurrentQueue(const ConcurrentQueue& src, const allocator_type& a)
      : ConcurrentQueue(a) {
    queue_rep_ptr->Assign(*src.queue_rep_ptr, allocator_, CopyConstructItem);
  }

  ConcurrentQueue(const ConcurrentQueue& src)
      : ConcurrentQueue(
            queue_allocator_traits::select_on_container_copy_construction(
                src.get_allocator())) {
    queue_rep_ptr->Assign(*src.queue_rep_ptr, allocator_, CopyConstructItem);
  }

  ConcurrentQueue(ConcurrentQueue&& src)
      : ConcurrentQueue(std::move(src.allocator_)) {
    InternalSwap(src);
  }

  ConcurrentQueue(ConcurrentQueue&& src, const allocator_type& a)
      : ConcurrentQueue(a) {
    if (allocator_ == src.allocator_) {
      InternalSwap(src);
    } else {
      queue_rep_ptr->Assign(*src.queue_rep_ptr, allocator_, MoveConstructItem);
      src.Clear();
    }
  }

  ~ConcurrentQueue() {
    Clear();
    queue_allocator_traits::destroy(allocator_, queue_rep_ptr);
    ws::concurrency::detail::CacheAlignedDeallocate(queue_rep_ptr);
  }

  ConcurrentQueue& operator=(const ConcurrentQueue& other) {
    if (queue_rep_ptr != other.queue_rep_ptr) {
      Clear();
      allocator_ = other.allocator_;
      queue_rep_ptr->Assign(*other.queue_rep_ptr, allocator_,
                            CopyConstructItem);
    }
    return *this;
  }

  ConcurrentQueue& operator=(ConcurrentQueue&& other) {
    if (queue_rep_ptr != other.queue_rep_ptr) {
      Clear();
      if (allocator_ == other.allocator_) {
        InternalSwap(other);
      } else {
        queue_rep_ptr->Assign(*other.queue_rep_ptr, other.allocator_,
                              MoveConstructItem);
        other.Clear();
        allocator_ = std::move(other.allocator_);
      }
    }
    return *this;
  }

  void Swap(ConcurrentQueue& other) {
    assert(allocator_ == other.allocator_ && "unequal allocators");
    InternalSwap(other);
  }

  void Push(const T& value) { InternalPush(value); }

  void Push(T&& value) { InternalPush(std::move(value)); }

  template <typename... TArgs>
  void Emplace(TArgs&&... args) {
    InternalPush(std::forward<TArgs>(args)...);
  }

  bool TryPop(T& result) { return InternalTryPop(&result); }

  size_type UnsafeSize() const {
    std::ptrdiff_t size = queue_rep_ptr->Size();
    return size < 0 ? 0 : size_type(size);
  }

  [[nodiscard]] bool Empty() const { return queue_rep_ptr->Empty(); }

  void Clear() { queue_rep_ptr->Clear(allocator_); }

  allocator_type get_allocator() const { return allocator_; }

 private:
  void InternalSwap(ConcurrentQueue& src) {
    using std::swap;
    swap(queue_rep_ptr, src.queue_rep_ptr);
  }

  template <typename... TArgs>
  void InternalPush(TArgs&&... args) {
    ws::concurrency::detail::ticket_type k = queue_rep_ptr->tail_counter_++;
    queue_rep_ptr->Choose(k).Push(k, *queue_rep_ptr, allocator_,
                                  std::forward<TArgs>(args)...);
  }

  bool InternalTryPop(void* dst) {
    return ws::concurrency::detail::InternalTryPopImpl(dst, *queue_rep_ptr,
                                                       allocator_)
        .first;
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
    lhs.Swap(rhs);
  }

  friend bool operator==(const ConcurrentQueue& lhs,
                         const ConcurrentQueue& rhs) {
    return lhs.UnsafeSize() == rhs.UnsafeSize();
  }

#if !_CPP20_COMPARISONS_PRESENT
  friend bool operator!=(const concurrent_queue& lhs,
                         const concurrent_queue& rhs) {
    return !(lhs == rhs);
  }
#endif
};

#if _CPP17_DEDUCTION_GUIDES_PRESENT >= 201606
template <typename TContainer, typename = std::allocator<TContainer>>
ConcurrentQueue(TContainer)
    -> ConcurrentQueue<typename TContainer::value_type, TContainer>;
#endif

namespace stl {
template <typename T, typename TAllocator = std::allocator<T>>
class concurrent_queue {
 public:
  using queue_type = ConcurrentQueue<T, TAllocator>;

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
  concurrent_queue() = default;

  explicit concurrent_queue(const allocator_type& alloc)
      : internal_instance_(alloc) {}

  template <typename TInputIt>
  concurrent_queue(TInputIt first, TInputIt last,
                   const allocator_type& alloc = allocator_type())
      : internal_instance_(first, last, alloc) {}

  concurrent_queue(const concurrent_queue& other)
      : internal_instance_(other.internal_instance_) {}

  concurrent_queue(concurrent_queue&& other) noexcept
      : internal_instance_(std::move(other.internal_instance_)) {}

  concurrent_queue(const concurrent_queue& other, const allocator_type& alloc)
      : internal_instance_(other.internal_instance_, alloc) {}

  concurrent_queue(concurrent_queue&& other,
                   const allocator_type& alloc) noexcept
      : internal_instance_(std::move(other.internal_instance_), alloc) {}

  [[nodiscard]] bool empty() const { return internal_instance_.Empty(); }

  [[nodiscard]]
  size_type size() const {
    return internal_instance_.UnsafeSize();
  }

  void push(const value_type& __x) { internal_instance_.Push(__x); }

  void push(value_type&& __x) { internal_instance_.Push(std::move(__x)); }

  template <typename... TArgs>
  void emplace(TArgs&&... __args) {
    internal_instance_.Emplace(std::forward<TArgs>(__args)...);
  }

  bool try_pop(value_type& __x) { return internal_instance_.TryPop(__x); }

  void pop(value_type& __x) { internal_instance_.TryPop(__x); }

  void pop() {
    value_type __x;
    internal_instance_.TryPop(__x);
  }

  void swap(concurrent_queue& __q) {
    internal_instance_.Swap(__q.internal_instance_);
  }

  concurrent_queue& operator=(const concurrent_queue& other) {
    internal_instance_ = other.internal_instance_;
    return *this;
  }

  concurrent_queue& operator=(concurrent_queue&& other) noexcept {
    internal_instance_ = std::move(other.internal_instance_);
    return *this;
  }
};

template <typename T, typename TAllocator>
void swap(concurrent_queue<T, TAllocator>& lhs,
          concurrent_queue<T, TAllocator>& rhs) noexcept {
  lhs.swap(rhs);
}

#if _CPP17_DEDUCTION_GUIDES_PRESENT >= 201606
template <typename TContainer, typename = std::allocator<TContainer>>
concurrent_queue(TContainer)
    -> concurrent_queue<typename TContainer::value_type, TContainer>;
#endif

}  // namespace stl
}  // namespace concurrency
}  // namespace ws
