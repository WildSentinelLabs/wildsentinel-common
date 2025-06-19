#pragma once

#include "status/status.h"
#include "status/status_code.h"
#include "wsexception.h"
namespace ws {
template <typename T>
class StatusOr {
 public:
  typedef T value_type;

  StatusOr(const T& value) : status_(), value_(value) {}

  StatusOr(T&& value) : status_(), value_(std::move(value)) {}

  StatusOr(const Status& status) : status_(status), value_(std::nullopt) {}

  StatusOr(const StatusOr&) = delete;

  StatusOr(StatusOr&&) noexcept
      : status_(std::move(status_)), value_(std::move(value_)) {};

  StatusOr& operator=(const StatusOr&) = delete;

  StatusOr& operator=(StatusOr&& other) noexcept {
    if (this != &other) {
      status_ = std::move(other.status_);
      value_ = std::move(other.value_);
    }

    return *this;
  };

  ~StatusOr() = default;

  bool Ok() const { return status_.Ok(); }

  const Status& GetStatus() const { return status_; }

  const T& Value() const& {
    EnsureValue();
    return *value_;
  }

  T& Value() & {
    EnsureValue();
    return *value_;
  }

  const T&& Value() const&& {
    EnsureValue();
    return std::move(*value_);
  }

  T&& Value() && {
    EnsureValue();
    return std::move(*value_);
  }

 private:
  Status status_;
  std::optional<T> value_;

  void EnsureValue() const {
    if (!Ok() || !value_.has_value()) WsException::BadStatusOrAccess().Throw();
  }
};

#define ASSIGN_OR_RETURN(lhs, expr)                \
  do {                                             \
    auto _result = (expr);                         \
    if (!_result.Ok()) return _result.GetStatus(); \
    lhs = std::move(_result.Value());              \
  } while (0)
}  // namespace ws
