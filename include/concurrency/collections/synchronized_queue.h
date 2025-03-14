#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
namespace ws {
namespace concurrency {
namespace collections {
template <typename T>
class SynchronizedQueue {
 public:
  using size_type = std::size_t;
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using difference_type = std::ptrdiff_t;

  SynchronizedQueue() = default;

  template <typename TInputIterator>
  SynchronizedQueue(TInputIterator begin, TInputIterator end) {
    for (; begin != end; ++begin) {
      Push(*begin);
    }
  }

  SynchronizedQueue(std::initializer_list<value_type> init)
      : SynchronizedQueue(init.begin(), init.end()) {}

  SynchronizedQueue(const SynchronizedQueue& other) {
    std::lock_guard<std::mutex> lock(other.mutex_);
    queue_ = other.queue_;
  }

  SynchronizedQueue& operator=(const SynchronizedQueue& other) {
    if (this != &other) {
      std::lock_guard<std::mutex> lock_this(mutex_);
      std::lock_guard<std::mutex> lock_other(other.mutex_);
      queue_ = other.queue_;
    }
    return *this;
  }

  SynchronizedQueue(SynchronizedQueue&& other) noexcept {
    std::lock_guard<std::mutex> lock(other.mutex_);
    queue_ = std::move(other.queue_);
  }

  SynchronizedQueue& operator=(SynchronizedQueue&& other) noexcept {
    if (this != &other) {
      std::lock_guard<std::mutex> lock_this(mutex_);
      std::lock_guard<std::mutex> lock_other(other.mutex_);
      queue_ = std::move(other.queue_);
    }
    return *this;
  }

  ~SynchronizedQueue() = default;

  void Push(const T& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(value);
    cv_.notify_one();
  }

  void Push(T&& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(std::move(value));
    cv_.notify_one();
  }

  template <typename... TArgs>
  void Emplace(TArgs&&... args) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.emplace(std::forward<TArgs>(args)...);
    cv_.notify_one();
  }

  void Pop(T& result) {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this]() { return !queue_.empty(); });
    result = std::move(queue_.front());
    queue_.pop();
  }

  bool TryPop(T& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) {
      return false;
    }
    value = std::move(queue_.front());
    queue_.pop();
    return true;
  }

  size_type UnsafeSize() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
  }

  bool Empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
  }

  void Clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::queue<T> empty_queue;
    std::swap(queue_, empty_queue);
  }

  void Swap(SynchronizedQueue& other) {
    if (this == &other) return;
    std::scoped_lock lock(mutex_, other.mutex_);
    queue_.swap(other.queue_);
  }

 private:
  std::queue<T> queue_;
  mutable std::mutex mutex_;
  std::condition_variable cv_;
};

namespace stl {

template <typename T>
class synchronized_queue {
 public:
  using queue_type = SynchronizedQueue<T>;

  using value_type = typename queue_type::value_type;
  using size_type = typename queue_type::size_type;
  using reference = typename queue_type::reference;
  using const_reference = typename queue_type::const_reference;
  using difference_type = typename queue_type::difference_type;

  using pointer = T*;
  using const_pointer = const T*;

 private:
  queue_type internal_instance_;

 public:
  explicit synchronized_queue() = default;

  template <typename TInputIt>
  synchronized_queue(TInputIt first, TInputIt last)
      : internal_instance_(first, last) {}

  synchronized_queue(std::initializer_list<value_type> init)
      : synchronized_queue(init.begin(), init.end()) {}

  synchronized_queue(const synchronized_queue& other)
      : internal_instance_(other.internal_instance_) {}

  synchronized_queue(synchronized_queue&& other) noexcept
      : internal_instance_(std::move(other.internal_instance_)) {}

  _GLIBCXX_NODISCARD bool empty() const { return internal_instance_.Empty(); }

  _GLIBCXX_NODISCARD size_type size() const {
    return internal_instance_.UnsafeSize();
  }

  void push(const value_type& x) { internal_instance_.Push(x); }

  void push(value_type&& x) { internal_instance_.Push(std::move(x)); }

  template <typename... TArgs>
  void emplace(TArgs&&... args) {
    internal_instance_.Emplace(std::forward<TArgs>(args)...);
  }

  bool try_pop(value_type& __x) { return internal_instance_.TryPop(__x); }

  void pop(value_type& __x) { internal_instance_.Pop(__x); }

  void pop() {
    value_type temp;
    internal_instance_.Pop(temp);
  }

  void swap(synchronized_queue& other) {
    internal_instance_.Swap(other.internal_instance_);
  }

  synchronized_queue& operator=(const synchronized_queue& other) {
    internal_instance_ = other.internal_instance_;
    return *this;
  }

  synchronized_queue& operator=(synchronized_queue&& other) noexcept {
    internal_instance_ = std::move(other.internal_instance_);
    return *this;
  }

  synchronized_queue& operator=(std::initializer_list<value_type> il) {
    internal_instance_ = il;
    return *this;
  }
};

template <typename T>
void swap(synchronized_queue<T>& lhs, synchronized_queue<T>& rhs) noexcept {
  lhs.swap(rhs);
}

}  // namespace stl

}  // namespace collections
}  // namespace concurrency
}  // namespace ws
