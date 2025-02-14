#include "logging/enrichers/process_id_enricher.h"
namespace ws {
namespace logging {
namespace enrichers {
void ProcessIdEnricher::Enrich(ws::logging::events::LogEvent& event) const {
  std::ostringstream oss;
  oss << getpid();
  event.AddProperty(kKey, oss.str());
}

const std::string ProcessIdEnricher::kKey = "ProcessId";
}  // namespace enrichers
}  // namespace logging
}  // namespace ws
