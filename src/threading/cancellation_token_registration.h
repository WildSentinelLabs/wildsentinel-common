#pragma once

#include "concurrency/collections/concurrent_unordered_map.h"
#include "threading/cancellation_state.h"
namespace ws {
namespace threading {
struct CancellationTokenRegistration {
 public:
  CancellationTokenRegistration() = default;
  CancellationTokenRegistration(std::weak_ptr<CancellationState> state,
                                size_t id);

  void Unregister();

 private:
  std::weak_ptr<CancellationState> state_;
  size_t id_;
};

// ============================================================================
// Implementation details for CancellationTokenRegistration
// ============================================================================

inline CancellationTokenRegistration::CancellationTokenRegistration(
    std::weak_ptr<CancellationState> state, size_t id)
    : state_(state), id_(id) {}

inline void CancellationTokenRegistration::Unregister() {
  if (auto st = state_.lock()) {
    st->callbacks.UnsafeErase(id_);
  }
}

}  // namespace threading
}  // namespace ws
