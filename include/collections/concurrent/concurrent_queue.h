#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T>
class ConcurrentQueue {
 public:
  ConcurrentQueue() = default;
  ~ConcurrentQueue() = default;

  void Push(const T& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(value);
    cv_.notify_one();
  }

  bool Pop(T& value) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (queue_.empty()) {
      return false;
    }
    value = std::move(queue_.front());
    queue_.pop();
    return true;
  }

  void WaitAndPop(T& value) {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return !queue_.empty(); });
    value = std::move(queue_.front());
    queue_.pop();
  }

  bool Empty() {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
  }

  size_t Size() {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
  }

 private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cv_;
};
