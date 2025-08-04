// Based on Abseil (https://github.com/abseil/abseil-cpp)
// See THIRD-PARTY-NOTICES

#pragma once

#include <optional>

#include "ws/status/status.h"
#include "ws/status/status_code.h"

namespace ws {
struct bad_status_or_access_exception : public std::exception {
  const char* what() const noexcept override;
};

template <typename T>
class StatusOr {
 public:
  typedef T value_type;

  StatusOr(const T& value);
  StatusOr(T&& value);
  StatusOr(const Status& status);
  StatusOr(const StatusOr&) = delete;
  StatusOr(StatusOr&&) noexcept;

  StatusOr& operator=(const StatusOr&) = delete;
  StatusOr& operator=(StatusOr&&) noexcept;

  ~StatusOr() = default;

  constexpr bool Ok() const;
  const Status& GetStatus() const;
  const T& Value() const&;
  T& Value() &;
  const T&& Value() const&&;
  T&& Value() &&;
  template <typename U = T>
  T ValueOr(U&& default_value) const&;
  template <typename U = T>
  T ValueOr(U&& default_value) &&;

  template <typename U>
  friend inline bool operator==(const StatusOr<U>&, const StatusOr<U>&);

  template <typename U>
  friend inline bool operator!=(const StatusOr<U>&, const StatusOr<U>&);

 private:
  void EnsureValue() const;

  Status status_;
  std::optional<T> value_;
};

// ============================================================================
// Implementation details for bad_status_or_access_exception
// ============================================================================

inline const char* bad_status_or_access_exception::what() const noexcept {
  return "Attempted to access value in StatusOr<T> that is not OK.";
}

// ============================================================================
// Implementation details for StatusOr<T>
// ============================================================================

template <typename T>
inline StatusOr<T>::StatusOr(const T& value) : status_(), value_(value) {}

template <typename T>
inline StatusOr<T>::StatusOr(T&& value) : status_(), value_(std::move(value)) {}

template <typename T>
inline StatusOr<T>::StatusOr(const Status& status)
    : status_(status), value_(std::nullopt) {}

template <typename T>
inline StatusOr<T>::StatusOr(StatusOr&& other) noexcept
    : status_(std::move(other.status_)), value_(std::move(other.value_)) {}

template <typename T>
inline StatusOr<T>& StatusOr<T>::operator=(StatusOr&& other) noexcept {
  if (this != &other) {
    status_ = std::move(other.status_);
    value_ = std::move(other.value_);
  }

  return *this;
}

template <typename T>
inline constexpr bool StatusOr<T>::Ok() const {
  return status_.Ok();
}

template <typename T>
inline const Status& StatusOr<T>::GetStatus() const {
  return status_;
}

template <typename T>
inline const T& StatusOr<T>::Value() const& {
  EnsureValue();
  return *value_;
}

template <typename T>
inline T& StatusOr<T>::Value() & {
  EnsureValue();
  return *value_;
}

template <typename T>
inline const T&& StatusOr<T>::Value() const&& {
  EnsureValue();
  return std::move(*value_);
}

template <typename T>
inline T&& StatusOr<T>::Value() && {
  EnsureValue();
  return std::move(*value_);
}

template <typename T>
template <typename U>
inline T StatusOr<T>::ValueOr(U&& default_value) const& {
  if (Ok() && value_.has_value()) return *value_;
  return static_cast<T>(std::forward<U>(default_value));
}

template <typename T>
template <typename U>
inline T StatusOr<T>::ValueOr(U&& default_value) && {
  if (Ok() && value_.has_value()) return std::move(*value_);
  return static_cast<T>(std::forward<U>(default_value));
}

template <typename T>
inline void StatusOr<T>::EnsureValue() const {
  if (!Ok() || !value_.has_value()) throw bad_status_or_access_exception();
}

template <typename U>
inline bool operator==(const StatusOr<U>& lhs, const StatusOr<U>& rhs) {
  return lhs.status_ == rhs.status_ && lhs.value_ == rhs.value_;
}

template <typename U>
inline bool operator!=(const StatusOr<U>& lhs, const StatusOr<U>& rhs) {
  return !(lhs == rhs);
}

#define ASSIGN_OR_RETURN(lhs, expr)                \
  do {                                             \
    auto _result = (expr);                         \
    if (!_result.Ok()) return _result.GetStatus(); \
    lhs = std::move(_result.Value());              \
  } while (0)

#define ASSIGN_OR_CLEANUP(lhs, expr, cleanup) \
  do {                                        \
    auto _result = (expr);                    \
    if (!_result.Ok()) {                      \
      cleanup;                                \
      return _result.GetStatus();             \
    }                                         \
    lhs = std::move(_result.Value());         \
  } while (0)

}  // namespace ws
