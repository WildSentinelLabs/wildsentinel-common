#pragma once

#include <cassert>

#include "ws/concurrency/collections/blocking_queue.h"
#include "ws/delegate.h"
#include "ws/pooling/iobject_pool.h"
#include "ws/status/status_or.h"

namespace ws {
namespace pooling {
template <typename T>
class BlockingObjectPool : public IObjectPool<T> {
 public:
  static StatusOr<BlockingObjectPool<T>> Create(
      ws::Delegate<T()> object_generator, std::size_t capacity);

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
};

// ============================================================================
// Implementation details for BlockingObjectPool<T>
// ============================================================================

template <typename T>
inline StatusOr<BlockingObjectPool<T>> BlockingObjectPool<T>::Create(
    ws::Delegate<T()> object_generator, std::size_t capacity) {
  if (!object_generator_) {
    return Status(StatusCode::kBadRequest, "object_generator is null");
  }

  if (capacity == 0) {
    return Status(StatusCode::kBadRequest,
                  "Pool capacity must be greater than 0");
  }

  return BlockingObjectPool<T>(object_generator, capacity);
}

template <typename T>
inline BlockingObjectPool<T>::BlockingObjectPool(
    ws::Delegate<T()> object_generator, std::size_t capacity)
    : object_generator_(object_generator),
      capacity_(capacity),
      current_count_(0) {
  queue_.SetCapacity(capacity_);
}

template <typename T>
inline T BlockingObjectPool<T>::Get() {
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
  queue_.Push(item);
}

template <typename T>
inline void BlockingObjectPool<T>::Return(T&& item) {
  queue_.Push(std::move(item));
}

template <typename T>
inline void BlockingObjectPool<T>::Clear() {
  if constexpr (!std::is_member_function_pointer<
                    decltype(&T::Dispose)>::value) {
    objects_.Clear();
    return;
  }

  while (objects_.TryPop(item)) {
    item.Dispose();
  }
}

template <typename T>
inline void BlockingObjectPool<T>::Dispose() {
  Clear();
  queue_.SetCapacity(0);
}

}  // namespace pooling
}  // namespace ws
