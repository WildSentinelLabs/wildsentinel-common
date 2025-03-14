#pragma once

#include <mutex>
#include <semaphore>

#include "arch/cpu_arch.h"
#include "concurrency/detail/aligned_space.h"

namespace ws {
namespace concurrency {
namespace detail {

template <typename TCondition>
bool TimedSpinWaitUntil(TCondition condition) {
  bool finish = condition();
  for (int i = 1; !finish && i < 32; finish = condition(), i *= 2) {
    ws::arch::detail::CpuWait(i);
  }
  for (int i = 32; !finish && i < 64; finish = condition(), ++i) {
    std::this_thread::yield();
  }
  return finish;
}

class ConcurrentMonitorMutex {
 public:
  using scoped_lock = std::lock_guard<ConcurrentMonitorMutex>;

  constexpr ConcurrentMonitorMutex() {}

  ~ConcurrentMonitorMutex() = default;

  void destroy() {
    if (init_flag_.load(std::memory_order_relaxed)) {
      Semaphore().~counting_semaphore<1>();
    }
  }

  void lock() {
    auto wake_up_condition = [&] {
      return flag_.load(std::memory_order_relaxed) == 0;
    };

    while (flag_.exchange(1)) {
      if (!TimedSpinWaitUntil(wake_up_condition)) {
        ++waiters_;
        while (!wake_up_condition()) {
          Wait();
        }
        --waiters_;
      }
    }
  }

  void unlock() {
    flag_.exchange(0);
    if (waiters_.load(std::memory_order_relaxed)) {
      WakeUp();
    }
  }

 private:
  void Wait() { Semaphore().acquire(); }

  void WakeUp() { Semaphore().release(); }

  std::atomic<int> flag_{0};
  std::atomic<int> waiters_{0};

  std::binary_semaphore& Semaphore() {
    if (!init_flag_.load(std::memory_order_acquire)) {
      std::lock_guard<std::mutex> lock(init_mutex_);
      if (!init_flag_.load(std::memory_order_relaxed)) {
        new (semaphore_.begin()) std::binary_semaphore(0);
        init_flag_.store(true, std::memory_order_release);
      }
    }

    return *semaphore_.begin();
  }

  inline static std::mutex init_mutex_;
  std::atomic<bool> init_flag_{false};
  AlignedSpace<std::binary_semaphore> semaphore_{};
};

}  // namespace detail
}  // namespace concurrency
}  // namespace ws
