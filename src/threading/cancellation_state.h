#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "delegate.h"
namespace ws {
namespace threading {
struct CancellationState {
  std::atomic_bool cancelled{false};
  std::mutex mtx;
  std::unordered_map<size_t, ws::Delegate<void()>> callbacks;
  size_t next_id = 1;
};
}  // namespace threading
}  // namespace ws
