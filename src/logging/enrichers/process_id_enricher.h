#pragma once

#include "format.h"
#include "logging/events/log_event.h"
#include "logging/ilog_enricher.h"
#include "machine.h"
namespace ws {
namespace logging {

class ProcessIdEnricher : public ILogEnricher {
 public:
  ProcessIdEnricher() = default;

  ~ProcessIdEnricher() override = default;

  void Enrich(ws::logging::LogEvent& event) const override;

 private:
  static const std::string kKey;
};

// ============================================================================
// Implementation details for ProcessIdEnricher
// ============================================================================

inline void ProcessIdEnricher::Enrich(ws::logging::LogEvent& event) const {
  event.AddProperty(kKey, Format("{}", GetPid()));
}

inline const std::string ProcessIdEnricher::kKey = "ProcessId";
}  // namespace logging
}  // namespace ws
