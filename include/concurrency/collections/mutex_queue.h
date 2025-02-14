#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
namespace ws {
namespace concurrency {
namespace collections {
template <typename T>
class MutexQueue {
 public:
  MutexQueue() = default;
  ~MutexQueue() = default;

  void Push(T&& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(std::move(value));
    cv_.notify_one();
  }

  void Push(const T& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(value);
    cv_.notify_one();
  }

  bool TryPop(T& value) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (queue_.empty()) {
      return false;
    }
    value = std::move(queue_.front());
    queue_.pop();
    return true;
  }

  void Clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::queue<T> empty_queue;
    std::swap(queue_, empty_queue);
  }

  void Swap(MutexQueue& other) {
    if (this == &other) return;
    std::lock_guard<std::mutex> lock_this(mutex_);
    std::lock_guard<std::mutex> lock_other(other.mutex_);
    queue_.swap(other.queue_);
  }

  bool Empty() {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
  }

 private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cv_;
};
}  // namespace collections
}  // namespace concurrency
}  // namespace ws
