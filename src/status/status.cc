#include "status/status.h"

namespace ws {
Status::Status() : code_(StatusCode::kOk), message_("OK") {}

Status::Status(StatusCode code, std::string message)
    : code_(code), message_(std::move(message)) {}

Status::Status(const Status& status)
    : code_(status.Code()), message_(status.Message()) {}

ws::Status::Status(Status&& other) noexcept
    : code_(other.code_), message_(std::move(other.message_)) {}

ws::Status& ws::Status::operator=(Status&& other) noexcept {
  if (this != &other) {
    code_ = other.code_;
    message_ = std::move(other.message_);
  }
  return *this;
}

bool Status::Ok() const { return code_ == StatusCode::kOk; }

StatusCode Status::Code() const { return code_; }

const std::string& Status::Message() const { return message_; }

std::string Status::ToString() const {
  return "[" + StatusCodeToString(code_) + "] " + message_;
}

bool operator==(const Status& lhs, const Status& rhs) {
  return lhs.code_ == rhs.code_ && lhs.message_ == rhs.message_;
}

bool operator!=(const Status& lhs, const Status& rhs) { return !(lhs == rhs); }

}  // namespace ws
