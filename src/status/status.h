#pragma once
#include "status/status_code.h"

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

  bool Ok() const;
  StatusCode Code() const;
  const std::string& Message() const;
  std::string ToString() const;

  friend bool operator==(const Status&, const Status&);
  friend bool operator!=(const Status&, const Status&);

 private:
  StatusCode code_;
  std::string message_;
};

#define RETURN_IF_ERROR(expr)          \
  do {                                 \
    auto _status = (expr);             \
    if (!_status.Ok()) return _status; \
  } while (0)

}  // namespace ws
