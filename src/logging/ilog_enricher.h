#pragma once
#include <string>

#include "logging/events/log_event.h"
namespace ws {
namespace logging {

class ILogEnricher {
 public:
  virtual void Enrich(ws::logging::events::LogEvent& event) const = 0;

  virtual ~ILogEnricher() = default;
};

template <typename T>
concept IsEnricher = std::is_base_of_v<ILogEnricher, T>;
}  // namespace logging
}  // namespace ws
