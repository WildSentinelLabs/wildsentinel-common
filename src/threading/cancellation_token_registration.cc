#include "threading/cancellation_token_registration.h"

CancellationTokenRegistration::CancellationTokenRegistration(
    std::weak_ptr<CancellationState> state, size_t id)
    : state_(state), id_(id) {}

void CancellationTokenRegistration::Unregister() {
  if (auto st = state_.lock()) {
    std::lock_guard<std::mutex> lock(st->mtx);
    st->callbacks.erase(id_);
  }
}
