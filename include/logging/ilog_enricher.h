#pragma once
#include <string>

#include "logging/events/log_event.h"

class ILogEnricher {
 public:
  virtual void Enrich(LogEvent& event) const = 0;

  virtual ~ILogEnricher() = default;
};
