#pragma once

#include "ws/logging/events/log_event.h"
#include "ws/logging/ilog_enricher.h"
#include "ws/machine.h"
#include "ws/string/format.h"
namespace ws {
namespace logging {

class ProcessIdEnricher : public ILogEnricher {
 public:
  ProcessIdEnricher() = default;

  ~ProcessIdEnricher() override = default;

  void Enrich(ws::logging::LogEvent& event) const override;

 private:
  static constexpr std::string_view kKey = "ProcessId";
};

// ============================================================================
// Implementation details for ProcessIdEnricher
// ============================================================================

inline void ProcessIdEnricher::Enrich(ws::logging::LogEvent& event) const {
  event.AddProperty(std::string(kKey), Format("{}", GetPid()));
}
}  // namespace logging
}  // namespace ws
