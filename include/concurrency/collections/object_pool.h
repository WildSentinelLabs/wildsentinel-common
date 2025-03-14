#pragma once

#include <functional>
#include <stdexcept>

#include "concurrency/collections/concurrent_queue.h"

namespace ws {
namespace concurrency {
namespace collections {
template <typename T>
class ObjectPool {
 public:
  explicit ObjectPool(std::function<T()> object_generator)
      : object_generator_(object_generator) {
    if (!object_generator_) {
      throw std::invalid_argument("object_generator is null");
    }
  }

  T Get() {
    T item;
    if (objects_.TryPop(item)) {
      return item;
    } else {
      return object_generator_();
    }
  }

  void Return(const T& item) { objects_.Push(item); }

  void Return(T&& item) { objects_.Push(std::move(item)); }

 private:
  ws::concurrency::collections::ConcurrentQueue<T> objects_;
  std::function<T()> object_generator_;
};
}  // namespace collections
}  // namespace concurrency
}  // namespace ws
