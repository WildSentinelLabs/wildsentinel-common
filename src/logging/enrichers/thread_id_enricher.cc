#include "logging/enrichers/thread_id_enricher.h"

void ThreadIdEnricher::Enrich(LogEvent& event) const {
  std::ostringstream oss;
  oss << std::this_thread::get_id();
  event.AddProperty(key_, oss.str());
}
