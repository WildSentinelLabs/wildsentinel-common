#pragma once

#include <cassert>
#include <functional>
#include <stdexcept>

#include "concurrency/collections/blocking_queue.h"
#include "delegate.h"
#include "idisposable.h"
#include "pooling/iobject_pool.h"

namespace ws {
namespace pooling {
template <ws::IsDisposable T>
class BlockingObjectPool : public IObjectPool<T> {
 public:
  explicit BlockingObjectPool(ws::Delegate<T()> object_generator,
                              std::size_t capacity)
      : object_generator_(object_generator),
        capacity_(capacity),
        current_count_(0),
        disposed_(false) {
    assert(capacity_ > 0 && "Pool capacity must not be 0 or less");
    if (!object_generator_) {
      throw std::invalid_argument("object_generator is null");
    }

    queue_.SetCapacity(capacity_);
  }

  T Get() override {
    if (disposed_.load(std::memory_order_acquire))
      throw disposed_object_exception();

    T item;
    if (queue_.TryPop(item)) {
      return item;
    }

    if (current_count_.load(std::memory_order_relaxed) < capacity_) {
      current_count_.fetch_add(1, std::memory_order_relaxed);
      return object_generator_();
    } else {
      queue_.Pop(item);
      return item;
    }
  }

  bool TryGet(T& item) { return queue_.TryPop(item); }

  void Return(const T& item) override {
    if (disposed_.load(std::memory_order_acquire))
      throw disposed_object_exception();

    queue_.Push(item);
  }

  void Return(T&& item) override {
    if (disposed_.load(std::memory_order_acquire))
      throw disposed_object_exception();

    queue_.Push(std::move(item));
  }

  void Dispose() override {
    if (disposed_.exchange(true, std::memory_order_acq_rel)) return;

    T item;
    while (queue_.TryPop(item)) {
      item.Dispose();
    }
  }

 private:
  const std::size_t capacity_;
  ws::concurrency::collections::BlockingQueue<T> queue_;
  ws::Delegate<T()> object_generator_;
  std::atomic<std::size_t> current_count_;
  std::atomic<bool> disposed_;
};
}  // namespace pooling
}  // namespace ws
