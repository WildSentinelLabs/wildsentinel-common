#pragma once

#include <atomic>
#include <stdexcept>

#include "concurrency/collections/concurrent_queue.h"
#include "delegate.h"
#include "pooling/iobject_pool.h"

namespace ws {
namespace pooling {

template <ws::IsDisposable T>
class ObjectPool : public IObjectPool<T> {
 public:
  explicit ObjectPool(ws::Delegate<T()> object_generator)
      : object_generator_(object_generator), disposed_(false) {
    if (!object_generator_) {
      throw std::invalid_argument("object_generator is null");
    }
  }

  T Get() override {
    if (disposed_.load()) throw disposed_object_exception();
    T item;
    if (objects_.TryPop(item)) {
      return item;
    } else {
      return object_generator_();
    }
  }

  void Return(const T& item) override {
    if (disposed_.load()) throw disposed_object_exception();
    objects_.Push(item);
  }

  void Return(T&& item) override {
    if (disposed_.load()) throw disposed_object_exception();
    objects_.Push(std::move(item));
  }

  void Dispose() override {
    if (disposed_.load()) throw disposed_object_exception();
    T item;
    while (objects_.TryPop(item)) {
      item.Dispose();
    }

    disposed_.store(true);
  }

 private:
  ws::concurrency::collections::ConcurrentQueue<T> objects_;
  ws::Delegate<T()> object_generator_;
  std::atomic<bool> disposed_;
};

}  // namespace pooling
}  // namespace ws
