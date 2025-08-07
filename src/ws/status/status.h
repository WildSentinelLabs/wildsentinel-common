// Based on Abseil (https://github.com/abseil/abseil-cpp)
// See THIRD-PARTY-NOTICES

#pragma once
#include "ws/status/status_code.h"

namespace ws {
class Status {
 public:
  Status();
  Status(StatusCode code, std::string message);
  Status(const Status&);
  Status(Status&&) noexcept;

  Status& operator=(const Status& x);
  Status& operator=(Status&&) noexcept;

  ~Status() = default;

  constexpr bool Ok() const;
  constexpr StatusCode Code() const;
  const std::string& Message() const;
  std::string ToString() const;

  static Status BadRequest(const std::string& message);
  static Status Unauthorized(const std::string& message);
  static Status Forbidden(const std::string& message);
  static Status NotFound(const std::string& message);
  static Status Conflict(const std::string& message);
  static Status InternalError(const std::string& message);
  static Status NotImplemented(const std::string& message);
  static Status RequestAborted(const std::string& message);
  static Status Timeout(const std::string& message);
  static Status PreconditionFailed(const std::string& message);
  static Status PayloadTooLarge(const std::string& message);
  static Status Unsupported(const std::string& message);
  static Status FailedDependency(const std::string& message);
  static Status Unavailable(const std::string& message);
  static Status InsufficientStorage(const std::string& message);
  static Status OutOfRange(const std::string& message);
  static Status BadAlloc(const std::string& message);
  static Status RuntimeError(const std::string& message);
  static Status DataLoss(const std::string& message);
  static Status Unknown(const std::string& message);

  friend inline bool operator==(const Status&, const Status&);
  friend inline bool operator!=(const Status&, const Status&);

 private:
  StatusCode code_;
  std::string message_;
};

// ============================================================================
// Implementation details for Status
// ============================================================================

inline Status::Status() : code_(StatusCode::kOk), message_("OK") {}

inline Status::Status(StatusCode code, std::string message)
    : code_(code), message_(std::move(message)) {}

inline Status::Status(const Status& status)
    : code_(status.Code()), message_(status.Message()) {}

inline ws::Status::Status(Status&& other) noexcept
    : code_(other.code_), message_(std::move(other.message_)) {}

inline constexpr bool Status::Ok() const { return code_ == StatusCode::kOk; }

inline constexpr StatusCode Status::Code() const { return code_; }

inline const std::string& Status::Message() const { return message_; }

inline bool operator==(const Status& lhs, const Status& rhs) {
  return lhs.code_ == rhs.code_ && lhs.message_ == rhs.message_;
}

inline bool operator!=(const Status& lhs, const Status& rhs) {
  return !(lhs == rhs);
}
}  // namespace ws

#define RETURN_IF_ERROR(expr)          \
  do {                                 \
    auto _status = (expr);             \
    if (!_status.Ok()) return _status; \
  } while (0)

#define CLEANUP_IF_ERROR(expr, cleanup) \
  do {                                  \
    auto _status = (expr);              \
    if (!_status.Ok()) {                \
      cleanup;                          \
      return _status;                   \
    }                                   \
  } while (0)
