#pragma once

#include <thread>

#include "ws/delegate.h"
#include "ws/threading/cancellation_state.h"
#include "ws/threading/cancellation_token.h"
namespace ws {
namespace threading {
class CancellationTokenSource {
 public:
  CancellationTokenSource();

  CancellationToken Token() const;
  void Cancel();
  void CancelAfter(std::chrono::milliseconds delay);

 private:
  std::shared_ptr<CancellationState> state_;
};

// ============================================================================
// Implementation details for CancellationTokenSource
// ============================================================================

inline CancellationTokenSource::CancellationTokenSource()
    : state_(std::make_shared<CancellationState>()) {}

inline CancellationToken CancellationTokenSource::Token() const {
  return CancellationToken(state_);
}
}  // namespace threading
}  // namespace ws
