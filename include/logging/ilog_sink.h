#pragma once

#include <memory>
#include <string>

#include "logging/events/log_event.h"
#include "logging/log_level.h"

class ILogSink {
 public:
  virtual void Enable() = 0;

  virtual void EnableAsync() = 0;

  virtual void Display(LogEvent event) = 0;

  virtual void Display(const std::string& message) = 0;

  virtual ~ILogSink() = default;
};
