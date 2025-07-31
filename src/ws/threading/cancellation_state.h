#pragma once

#include <atomic>
#include <memory>

#include "ws/concurrency/collections/concurrent_unordered_map.h"
#include "ws/delegate.h"
namespace ws {
namespace threading {
struct CancellationState {
  using key_type = std::size_t;
  using mapped_type = Delegate<void()>;
  using map_type =
      ws::concurrency::stl::concurrent_unordered_map<key_type, mapped_type>;

  std::atomic_bool cancelled{false};
  map_type callbacks;
  size_t next_id = 1;
};
}  // namespace threading
}  // namespace ws
