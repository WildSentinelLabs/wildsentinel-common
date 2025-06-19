#include "threading/cancellation_token_source.h"
namespace ws {
namespace threading {
void CancellationTokenSource::Cancel() {
  if (!state_) return;

  std::unordered_map<size_t, ws::Delegate<void()>> callbacks_copy;
  {
    std::lock_guard<std::mutex> lock(state_->mtx);
    if (state_->cancelled.load()) return;
    state_->cancelled.store(true);
    callbacks_copy = state_->callbacks;
    state_->callbacks.clear();
  }

  for (const auto& pair : callbacks_copy) {
    try {
      pair.second();
    } catch (...) {
    }
  }
}

void CancellationTokenSource::CancelAfter(std::chrono::milliseconds delay) {
  std::thread([this, delay]() {
    std::this_thread::sleep_for(delay);
    Cancel();
  }).detach();
}
}  // namespace threading
}  // namespace ws
