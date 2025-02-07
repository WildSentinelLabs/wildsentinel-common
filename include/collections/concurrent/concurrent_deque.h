#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>

template <typename T>
class ConcurrentDeque {
 public:
  ConcurrentDeque() = default;
  ~ConcurrentDeque() = default;

  void PushBack(const T& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    deque_.push_back(value);
    cv_.notify_one();
  }

  void PushFront(const T& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    deque_.push_front(value);
    cv_.notify_one();
  }

  bool PopFront(T& value) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (deque_.empty()) {
      return false;
    }
    value = std::move(deque_.front());
    deque_.pop_front();
    return true;
  }

  bool PopBack(T& value) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (deque_.empty()) {
      return false;
    }
    value = std::move(deque_.back());
    deque_.pop_back();
    return true;
  }

  void WaitAndPopFront(T& value) {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return !deque_.empty(); });
    value = std::move(deque_.front());
    deque_.pop_front();
  }

  bool Empty() {
    std::lock_guard<std::mutex> lock(mutex_);
    return deque_.empty();
  }

  size_t Size() {
    std::lock_guard<std::mutex> lock(mutex_);
    return deque_.size();
  }

 private:
  std::deque<T> deque_;
  std::mutex mutex_;
  std::condition_variable cv_;
};
