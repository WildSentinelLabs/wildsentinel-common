#include "threading/cancellation_token_source.h"
namespace ws {
namespace threading {
void CancellationTokenSource::Cancel() {
  if (state_->cancelled.exchange(true)) return;

  auto callbacks_copy = state_->callbacks;
  state_->callbacks.Clear();

  for (const auto& [id, callback] : callbacks_copy) {
    try {
      callback();
    } catch (...) {
      // Handle exceptions
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
