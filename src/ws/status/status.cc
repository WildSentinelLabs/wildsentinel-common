// Based on Abseil (https://github.com/abseil/abseil-cpp)
// See THIRD-PARTY-NOTICES

#include "ws/status/status.h"

namespace ws {
Status& Status::operator=(const Status& other) {
  if (this != &other) {
    code_ = other.code_;
    message_ = other.message_;
  }

  return *this;
}

ws::Status& ws::Status::operator=(Status&& other) noexcept {
  if (this != &other) {
    code_ = other.code_;
    message_ = std::move(other.message_);
  }
  return *this;
}

std::string Status::ToString() const {
  return "[" + StatusCodeToString(code_) + "] " + message_;
}

Status Status::BadRequest(const std::string& message) {
  return Status(StatusCode::kBadRequest, message);
}

Status Status::Unauthorized(const std::string& message) {
  return Status(StatusCode::kUnauthorized, message);
}

Status Status::Forbidden(const std::string& message) {
  return Status(StatusCode::kForbidden, message);
}

Status Status::NotFound(const std::string& message) {
  return Status(StatusCode::kNotFound, message);
}

Status Status::Conflict(const std::string& message) {
  return Status(StatusCode::kConflict, message);
}

Status Status::InternalError(const std::string& message) {
  return Status(StatusCode::kInternalError, message);
}

Status Status::NotImplemented(const std::string& message) {
  return Status(StatusCode::kNotImplemented, message);
}

Status Status::RequestAborted(const std::string& message) {
  return Status(StatusCode::kRequestAborted, message);
}

Status Status::Timeout(const std::string& message) {
  return Status(StatusCode::kTimeout, message);
}

Status Status::PreconditionFailed(const std::string& message) {
  return Status(StatusCode::kPreconditionFailed, message);
}

Status Status::PayloadTooLarge(const std::string& message) {
  return Status(StatusCode::kPayloadTooLarge, message);
}

Status Status::Unsupported(const std::string& message) {
  return Status(StatusCode::kUnsupported, message);
}

Status Status::FailedDependency(const std::string& message) {
  return Status(StatusCode::kFailedDependency, message);
}

Status Status::Unavailable(const std::string& message) {
  return Status(StatusCode::kUnavailable, message);
}

Status Status::InsufficientStorage(const std::string& message) {
  return Status(StatusCode::kInsufficientStorage, message);
}

Status Status::OutOfRange(const std::string& message) {
  return Status(StatusCode::kOutOfRange, message);
}

Status Status::BadAlloc(const std::string& message) {
  return Status(StatusCode::kBadAlloc, message);
}

Status Status::RuntimeError(const std::string& message) {
  return Status(StatusCode::kRuntimeError, message);
}

Status Status::DataLoss(const std::string& message) {
  return Status(StatusCode::kDataLoss, message);
}

Status Status::Unknown(const std::string& message) {
  return Status(StatusCode::kUnknown, message);
}
}  // namespace ws
