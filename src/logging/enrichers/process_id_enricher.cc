#include "logging/enrichers/process_id_enricher.h"
namespace ws {
namespace logging {

void ProcessIdEnricher::Enrich(ws::logging::LogEvent& event) const {
  event.AddProperty(kKey, Format("{}", GetPid()));
}

const std::string ProcessIdEnricher::kKey = "ProcessId";

}  // namespace logging
}  // namespace ws
