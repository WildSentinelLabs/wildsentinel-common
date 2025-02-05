#include "threading/cancellation_token_source.h"

CancellationTokenSource::CancellationTokenSource()
    : state_(std::make_shared<CancellationState>()) {}

CancellationToken CancellationTokenSource::Token() const {
  return CancellationToken(state_);
}

void CancellationTokenSource::Cancel() {
  if (!state_) return;

  std::unordered_map<size_t, std::function<void()>> callbacksCopy;
  {
    std::lock_guard<std::mutex> lock(state_->mtx);
    if (state_->cancelled.load()) return;
    state_->cancelled.store(true);
    callbacksCopy = state_->callbacks;
    state_->callbacks.clear();
  }

  for (const auto& pair : callbacksCopy) {
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
