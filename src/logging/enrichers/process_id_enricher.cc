#include "logging/enrichers/process_id_enricher.h"
namespace ws {
namespace logging {
namespace enrichers {
void ProcessIdEnricher::Enrich(ws::logging::events::LogEvent& event) const {
  event.AddProperty(kKey, std::format("{}", ws::arch::GetPid()));
}

const std::string ProcessIdEnricher::kKey = "ProcessId";
}  // namespace enrichers
}  // namespace logging
}  // namespace ws
