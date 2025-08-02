// Based on oneTBB (https://github.com/uxlfoundation/oneTBB)
// See THIRD-PARTY-NOTICES

#pragma once

#include "ws/config.h"
#include "ws/machine.h"

namespace ws {
namespace concurrency {
namespace internal {
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
      ws::internal::CpuWait(count_);
      count_ *= 2;
    } else {
      std::this_thread::yield();
    }
  }

  bool BoundedWait() {
    ws::internal::CpuWait(count_);
    if (count_ < kLoopsBeforeYield) {
      count_ *= 2;
      return true;
    } else {
      return false;
    }
  }

  void Reset() { count_ = 1; }
};
}  // namespace internal
}  // namespace concurrency
}  // namespace ws
