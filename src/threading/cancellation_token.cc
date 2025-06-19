#include "threading/cancellation_token.h"

namespace ws {
namespace threading {
CancellationTokenRegistration CancellationToken::RegisterCallback(
    const ws::Delegate<void()>& callback) {
  if (!state_) {
    WsException::InvalidArgument("Cancellation token not initialized.").Throw();
  }

  if (state_->cancelled.load()) {
    callback();
    return CancellationTokenRegistration();
  }

  {
    std::lock_guard<std::mutex> lock(state_->mtx);
    if (state_->cancelled.load()) {
      callback();
      return CancellationTokenRegistration();
    }

    long id = state_->next_id++;
    state_->callbacks[id] = callback;
    return CancellationTokenRegistration(state_, id);
  }
}
}  // namespace threading
}  // namespace ws
