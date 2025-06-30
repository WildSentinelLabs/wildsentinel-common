#include "threading/cancellation_token.h"

namespace ws {
namespace threading {
StatusOr<CancellationTokenRegistration> CancellationToken::RegisterCallback(
    const ws::Delegate<void()>& callback) {
  if (!state_)
    return Status(StatusCode::kBadRequest,
                  "Cancellation token not initialized.");

  if (state_->cancelled.load()) {
    callback();
    return CancellationTokenRegistration();
  }

  long id = state_->next_id++;
  state_->callbacks[id] = callback;
  return CancellationTokenRegistration(state_, id);
}
}  // namespace threading
}  // namespace ws
