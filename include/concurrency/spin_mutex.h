#pragma once

#include "concurrency/detail/atomic_backoff.h"

namespace ws {
namespace concurrency {
class SpinMutex {
 public:
  SpinMutex() noexcept : m_flag_(false) {}

  ~SpinMutex() = default;

  SpinMutex(const SpinMutex&) = delete;
  SpinMutex& operator=(const SpinMutex&) = delete;

  class ScopedLock {
   public:
    explicit ScopedLock(SpinMutex& mutex) : m_mutex(mutex) { m_mutex.Lock(); }
    ~ScopedLock() { m_mutex.Unlock(); }

    ScopedLock(const ScopedLock&) = delete;
    ScopedLock& operator=(const ScopedLock&) = delete;

   private:
    SpinMutex& m_mutex;
  };

  static constexpr bool kIsRwMutex = false;
  static constexpr bool kIsRecursiveMutex = false;
  static constexpr bool IsFairMutex = false;

  void Lock() {
    while (m_flag_.exchange(true, std::memory_order_acquire)) {
      while (m_flag_.load(std::memory_order_relaxed)) {
        std::this_thread::yield();
      }
    }
  }

  bool TryLock() { return !m_flag_.exchange(true, std::memory_order_acquire); }

  void Unlock() { m_flag_.store(false, std::memory_order_release); }

 private:
  std::atomic<bool> m_flag_;
};

template <typename T, typename C>
T SpinWaitWhile(const std::atomic<T>& location, C comp,
                std::memory_order order) {
  ws::concurrency::detail::AtomicBackoff backoff;
  T snapshot = location.load(order);
  while (comp(snapshot)) {
    backoff.Wait();
    snapshot = location.load(order);
  }
  return snapshot;
}

template <typename T, typename U>
T SpinWaitWhileEq(const std::atomic<T>& location, const U value,
                  std::memory_order order = std::memory_order_acquire) {
  return SpinWaitWhile(location, [&value](T t) { return t == value; }, order);
}

template <typename T, typename U>
T SpinWaitUntilEq(const std::atomic<T>& location, const U value,
                  std::memory_order order = std::memory_order_acquire) {
  return SpinWaitWhile(location, [&value](T t) { return t != value; }, order);
}
}  // namespace concurrency
}  // namespace ws
