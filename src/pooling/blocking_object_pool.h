#pragma once

#include <cassert>

#include "concurrency/collections/blocking_queue.h"
#include "delegate.h"
#include "idisposable.h"
#include "pooling/iobject_pool.h"
#include "wsexception.h"

namespace ws {
namespace pooling {
template <typename T>
class BlockingObjectPool : public IObjectPool<T> {
 public:
  explicit BlockingObjectPool(ws::Delegate<T()> object_generator,
                              std::size_t capacity);

  T Get() override;
  bool TryGet(T& item);
  void Return(const T& item) override;
  void Return(T&& item) override;
  void Clear() override;
  void Dispose();

 private:
  const std::size_t capacity_;
  ws::concurrency::BlockingQueue<T> queue_;
  ws::Delegate<T()> object_generator_;
  std::atomic<std::size_t> current_count_;
  std::atomic<bool> disposed_;
};

// ============================================================================
// Implementation details for BlockingObjectPool<T>
// ============================================================================

template <typename T>
inline BlockingObjectPool<T>::BlockingObjectPool(
    ws::Delegate<T()> object_generator, std::size_t capacity)
    : object_generator_(object_generator),
      capacity_(capacity),
      current_count_(0),
      disposed_(false) {
  assert(capacity_ > 0 && "Pool capacity must not be 0 or less");
  if (!object_generator_) {
    WsException::InvalidArgument("object_generator is null").Throw();
  }

  queue_.SetCapacity(capacity_);
}

template <typename T>
inline T BlockingObjectPool<T>::Get() {
  if (disposed_.load(std::memory_order_acquire))
    WsException::DisposedObject().Throw();

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

template <typename T>
inline bool BlockingObjectPool<T>::TryGet(T& item) {
  return queue_.TryPop(item);
}

template <typename T>
inline void BlockingObjectPool<T>::Return(const T& item) {
  if (disposed_.load(std::memory_order_acquire))
    WsException::DisposedObject().Throw();

  queue_.Push(item);
}

template <typename T>
inline void BlockingObjectPool<T>::Return(T&& item) {
  if (disposed_.load(std::memory_order_acquire))
    WsException::DisposedObject().Throw();

  queue_.Push(std::move(item));
}

template <typename T>
inline void BlockingObjectPool<T>::Clear() {
  if (disposed_.load(std::memory_order_acquire))
    WsException::DisposedObject().Throw();
  if constexpr (!std::is_member_function_pointer<
                    decltype(&T::Dispose)>::value) {
    objects_.Clear();
    return;
  }

  while (objects_.TryPop(item)) {
    if (disposed_.load()) WsException::DisposedObject().Throw();
    item.Dispose();
  }
}

template <typename T>
inline void BlockingObjectPool<T>::Dispose() {
  Clear();
  queue_.SetCapacity(0);
  disposed_.store(true);
}

}  // namespace pooling
}  // namespace ws
