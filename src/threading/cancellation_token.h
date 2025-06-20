#pragma once
#include <memory>

#include "delegate.h"
#include "threading/cancellation_state.h"
#include "threading/cancellation_token_registration.h"
#include "wsexception.h"
namespace ws {
namespace threading {

struct CancellationToken {
 public:
  static CancellationToken None();

  CancellationToken() = default;
  CancellationToken(std::shared_ptr<CancellationState> state);

  bool IsCancellationRequested() const;
  CancellationTokenRegistration RegisterCallback(
      const ws::Delegate<void()>& callback);

  void ThrowIfCancellationRequested() const;

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

inline void CancellationToken::ThrowIfCancellationRequested() const {
  if (IsCancellationRequested()) WsException::OperationCanceled().Throw();
}
}  // namespace threading
}  // namespace ws
