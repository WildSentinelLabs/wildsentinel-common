#pragma once

#include <memory>
#include <string>

#include "logging/events/log_event.h"
#include "logging/log_level.h"
namespace ws {
namespace logging {
class ILogSink {
 public:
  virtual ~ILogSink() = default;

  virtual void Enable() = 0;
  virtual void EnableAsync() = 0;
  virtual void Display(ws::logging::LogEvent event) = 0;
  virtual void Display(const std::string& message) const = 0;
};
}  // namespace logging
}  // namespace ws
