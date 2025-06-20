#pragma once

#include <atomic>
#include <memory>

#include "concurrency/collections/concurrent_unordered_map.h"
#include "delegate.h"
namespace ws {
namespace threading {
struct CancellationState {
  std::atomic_bool cancelled{false};
  ws::concurrency::ConcurrentUnorderedMap<size_t, ws::Delegate<void()>>
      callbacks;
  size_t next_id = 1;
};
}  // namespace threading
}  // namespace ws
