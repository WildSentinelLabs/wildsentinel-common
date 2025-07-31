#pragma once

#include <string>

#include "ws/logging/events/log_event.h"
#include "ws/logging/ilog_sink.h"
#include "ws/logging/log_level.h"
namespace ws {
namespace logging {
class ILogDispatcher {
 public:
  virtual ~ILogDispatcher() = default;

  virtual void Dispatch(const ILogSink& sink, const std::string& message) = 0;
  virtual void Await() = 0;
};
}  // namespace logging
}  // namespace ws
