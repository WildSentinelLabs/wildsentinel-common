#pragma once

#include <atomic>

#include "ws/concurrency/collections/concurrent_queue.h"
#include "ws/delegate.h"
#include "ws/pooling/iobject_pool.h"
#include "ws/status/status_or.h"

namespace ws {
namespace pooling {
template <typename T>
class ObjectPool : public IObjectPool<T> {
 public:
  static StatusOr<ObjectPool<T>> Create(ws::Delegate<T()> object_generator);

  ObjectPool<T>() noexcept = default;

  ~ObjectPool<T>();

  T Get() override;
  void Return(const T& item) override;
  void Return(T&& item) override;
  void Clear() override;

 private:
  explicit ObjectPool(ws::Delegate<T()> object_generator);

  ws::concurrency::ConcurrentQueue<T> objects_;
  ws::Delegate<T()> object_generator_;
};

// ============================================================================
// Implementation details for ObjectPool<T>
// ============================================================================

template <typename T>
inline StatusOr<ObjectPool<T>> ObjectPool<T>::Create(
    ws::Delegate<T()> object_generator) {
  if (!object_generator_)
    return Status(StatusCode::kBadRequest, "Object generator cannot be null");
  return ObjectPool<T>(object_generator);
}

template <typename T>
inline ObjectPool<T>::ObjectPool(ws::Delegate<T()> object_generator)
    : object_generator_(object_generator) {}

template <typename T>
inline ObjectPool<T>::~ObjectPool() {
  Clear();
}

template <typename T>
inline T ObjectPool<T>::Get() {
  T item;
  if (objects_.TryPop(item)) {
    return item;
  } else {
    return object_generator_();
  }
}

template <typename T>
inline void ObjectPool<T>::Return(const T& item) {
  objects_.Push(item);
}

template <typename T>
inline void ObjectPool<T>::Return(T&& item) {
  objects_.Push(std::move(item));
}

template <typename T>
inline void ObjectPool<T>::Clear() {
  if constexpr (!std::is_member_function_pointer<
                    decltype(&T::Dispose)>::value) {
    objects_.Clear();
    return;
  }

  while (objects_.TryPop(item)) {
    item.Dispose();
  }
}
}  // namespace pooling
}  // namespace ws
