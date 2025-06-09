#pragma once

#include <thread>

#include "threading/cancellation_state.h"
#include "threading/cancellation_token.h"
namespace ws {
namespace threading {

class CancellationTokenSource {
 public:
  CancellationTokenSource();

  CancellationToken Token() const;

  void Cancel();

  void CancelAfter(std::chrono::milliseconds delay);

 private:
  std::shared_ptr<CancellationState> state_;
};
}  // namespace threading
}  // namespace ws
