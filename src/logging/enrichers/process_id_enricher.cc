#include "logging/enrichers/process_id_enricher.h"

void ProcessIdEnricher::Enrich(LogEvent& event) const {
  std::ostringstream oss;
  oss << getpid();
  event.AddProperty(kKey, oss.str());
}

const std::string ProcessIdEnricher::kKey = "ProcessId";
