#include "logging/enrichers/process_id_enricher.h"

void ProcessIdEnricher::Enrich(LogEvent& event) const {
  std::ostringstream oss;
  oss << getpid();
  event.AddProperty(key_, oss.str());
}
