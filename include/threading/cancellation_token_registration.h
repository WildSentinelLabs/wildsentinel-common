#pragma once

#include "threading/cancellation_state.h"

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
