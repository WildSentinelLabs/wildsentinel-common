#include "logging/enrichers/thread_id_enricher.h"

void ThreadIdEnricher::Enrich(LogEvent& event) const {
  std::ostringstream oss;
  oss << std::this_thread::get_id();
  event.AddProperty(kKey, oss.str());
}

const std::string ThreadIdEnricher::kKey = "ThreadId";
