#pragma once
#include <memory>

#include "threading/cancellation_state.h"
#include "threading/cancellation_token_registration.h"
#include "wsexception.h"
namespace ws {
namespace threading {

struct CancellationToken {
 public:
  static CancellationToken None();

  CancellationToken() = default;

  explicit CancellationToken(std::shared_ptr<CancellationState> state);

  bool IsCancellationRequested() const;

  CancellationTokenRegistration RegisterCallback(
      const std::function<void()>& callback);

  void ThrowIfCancellationRequested() const;

 private:
  std::shared_ptr<CancellationState> state_;
  friend class CancellationTokenSource;
};
}  // namespace threading
}  // namespace ws
