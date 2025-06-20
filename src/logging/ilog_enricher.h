#pragma once

#include <string>

#include "logging/events/log_event.h"
namespace ws {
namespace logging {
class ILogEnricher {
 public:
  virtual ~ILogEnricher() = default;

  virtual void Enrich(ws::logging::LogEvent& event) const = 0;
};

template <typename T>
concept IsEnricher = std::is_base_of_v<ILogEnricher, T>;
}  // namespace logging
}  // namespace ws
