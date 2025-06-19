#pragma once

#include "threading/cancellation_state.h"
namespace ws {
namespace threading {
struct CancellationTokenRegistration {
 public:
  CancellationTokenRegistration() = default;
  CancellationTokenRegistration(std::weak_ptr<CancellationState> state,
                                size_t id);

  void Unregister();

 private:
  std::weak_ptr<CancellationState> state_;
  size_t id_;
};

inline CancellationTokenRegistration::CancellationTokenRegistration(
    std::weak_ptr<CancellationState> state, size_t id)
    : state_(state), id_(id) {}

inline void CancellationTokenRegistration::Unregister() {
  if (auto st = state_.lock()) {
    std::lock_guard<std::mutex> lock(st->mtx);
    st->callbacks.erase(id_);
  }
}

}  // namespace threading
}  // namespace ws
