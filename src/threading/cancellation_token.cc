#include "threading/cancellation_token.h"

#include <memory>
namespace ws {
namespace threading {

CancellationToken CancellationToken::None() { return CancellationToken(); }

CancellationToken::CancellationToken(std::shared_ptr<CancellationState> state)
    : state_(state) {}

bool CancellationToken::IsCancellationRequested() const {
  return state_ && state_->cancelled.load();
}

CancellationTokenRegistration CancellationToken::RegisterCallback(
    const std::function<void()>& callback) {
  if (!state_) {
    throw std::runtime_error("Cancellation token not initialized.");
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

void CancellationToken::ThrowIfCancellationRequested() const {
  if (IsCancellationRequested()) throw std::runtime_error("Operation canceled");
}
}  // namespace threading
}  // namespace ws
