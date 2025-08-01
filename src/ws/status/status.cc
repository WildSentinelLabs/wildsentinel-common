// Based on Abseil (https://github.com/abseil/abseil-cpp)
// See THIRD-PARTY-NOTICES

#include "ws/status/status.h"

namespace ws {
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

}  // namespace ws
