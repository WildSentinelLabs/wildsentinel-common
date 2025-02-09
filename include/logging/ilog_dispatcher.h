#pragma once

#include <string>

#include "logging/events/log_event.h"
#include "logging/log_level.h"

class ILogDispatcher {
 public:
  virtual void Dispatch(const std::string& message) = 0;

  virtual void Await() = 0;

  virtual ~ILogDispatcher() = default;
};
