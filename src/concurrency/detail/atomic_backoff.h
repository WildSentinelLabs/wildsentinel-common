#pragma once

#include "arch/config.h"
#include "arch/cpu_arch.h"

namespace ws {
namespace concurrency {
namespace detail {
class AtomicBackoff {
  static constexpr std::int32_t kLoopsBeforeYield = 16;
  std::int32_t count_;

 public:
  AtomicBackoff() : count_(1) {}
  AtomicBackoff(bool) : count_(1) { Wait(); }

  AtomicBackoff(const AtomicBackoff&) = delete;
  AtomicBackoff& operator=(const AtomicBackoff&) = delete;

  void Wait() {
    if (count_ <= kLoopsBeforeYield) {
      ws::detail::CpuWait(count_);
      count_ *= 2;
    } else {
      std::this_thread::yield();
    }
  }

  bool BoundedWait() {
    ws::detail::CpuWait(count_);
    if (count_ < kLoopsBeforeYield) {
      count_ *= 2;
      return true;
    } else {
      return false;
    }
  }

  void Reset() { count_ = 1; }
};
}  // namespace detail
}  // namespace concurrency
}  // namespace ws
