#pragma once
#include <memory>

#include "ws/delegate.h"
#include "ws/status/status_or.h"
#include "ws/threading/cancellation_state.h"
#include "ws/threading/cancellation_token_registration.h"
namespace ws {
namespace threading {
struct CancellationToken {
 public:
  static CancellationToken None();

  CancellationToken() = default;
  CancellationToken(std::shared_ptr<CancellationState> state);

  bool IsCancellationRequested() const;
  StatusOr<CancellationTokenRegistration> RegisterCallback(
      const ws::Delegate<void()>& callback);

 private:
  friend class CancellationTokenSource;

  std::shared_ptr<CancellationState> state_;
};

// ============================================================================
// Implementation details for CancellationToken
// ============================================================================

inline CancellationToken CancellationToken::None() {
  return CancellationToken();
}

inline CancellationToken::CancellationToken(
    std::shared_ptr<CancellationState> state)
    : state_(state) {}

inline bool CancellationToken::IsCancellationRequested() const {
  return state_ && state_->cancelled.load();
}
}  // namespace threading
}  // namespace ws
