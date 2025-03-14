#pragma once

#include <functional>
#include <stdexcept>

#include "concurrency/collections/blocking_queue.h"

namespace ws {
namespace concurrency {
namespace collections {

template <typename T>
class BlockingObjectPool {
 public:
  explicit BlockingObjectPool(std::function<T()> object_generator,
                              std::size_t max_items)
      : object_generator_(object_generator),
        max_items_(max_items),
        current_count_(0) {
    if (!object_generator_) {
      throw std::invalid_argument("object_generator is null");
    }

    queue_.SetCapacity(max_items_);
  }

  T Get() {
    T item;
    if (queue_.TryPop(item)) {
      return item;
    }

    if (current_count_.load(std::memory_order_relaxed) < max_items_) {
      current_count_.fetch_add(1, std::memory_order_relaxed);
      return object_generator_();
    } else {
      queue_.Pop(item);
      return item;
    }
  }

  void Return(const T& item) { queue_.Push(item); }

  void Return(T&& item) { queue_.Push(std::move(item)); }

 private:
  std::function<T()> object_generator_;
  ws::concurrency::collections::BlockingQueue<T> queue_;
  const std::size_t max_items_;
  std::atomic<std::size_t> current_count_;
};

}  // namespace collections
}  // namespace concurrency
}  // namespace ws
