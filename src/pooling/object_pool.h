#pragma once

#include <atomic>

#include "concurrency/collections/concurrent_queue.h"
#include "delegate.h"
#include "pooling/iobject_pool.h"
#include "wsexception.h"

namespace ws {
namespace pooling {
template <typename T>
class ObjectPool : public IObjectPool<T> {
 public:
  explicit ObjectPool(ws::Delegate<T()> object_generator);

  T Get() override;
  void Return(const T& item) override;
  void Return(T&& item) override;
  void Dispose() override;
  void Clear() override;

 private:
  ws::concurrency::ConcurrentQueue<T> objects_;
  ws::Delegate<T()> object_generator_;
  std::atomic<bool> disposed_;
};

// ============================================================================
// Implementation details for ObjectPool<T>
// ============================================================================

template <typename T>
inline ObjectPool<T>::ObjectPool(ws::Delegate<T()> object_generator)
    : object_generator_(object_generator), disposed_(false) {
  if (!object_generator_) {
    WsException::InvalidArgument("object_generator is null").Throw();
  }
}

template <typename T>
inline T ObjectPool<T>::Get() {
  if (disposed_.load()) WsException::DisposedObject().Throw();
  T item;
  if (objects_.TryPop(item)) {
    return item;
  } else {
    return object_generator_();
  }
}

template <typename T>
inline void ObjectPool<T>::Return(const T& item) {
  if (disposed_.load()) WsException::DisposedObject().Throw();
  objects_.Push(item);
}

template <typename T>
inline void ObjectPool<T>::Return(T&& item) {
  if (disposed_.load()) WsException::DisposedObject().Throw();
  objects_.Push(std::move(item));
}

template <typename T>
inline void ObjectPool<T>::Clear() {
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
inline void ObjectPool<T>::Dispose() {
  Clear();
  queue_.SetCapacity(0);
  disposed_.store(true);
}

}  // namespace pooling
}  // namespace ws
