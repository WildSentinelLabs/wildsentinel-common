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
}  // namespace threading
}  // namespace ws
