#include "logging/enrichers/thread_id_enricher.h"
namespace ws {
namespace logging {
namespace enrichers {
void ThreadIdEnricher::Enrich(ws::logging::events::LogEvent& event) const {
  std::ostringstream oss;
  oss << std::this_thread::get_id();
  event.AddProperty(kKey, oss.str());
}

const std::string ThreadIdEnricher::kKey = "ThreadId";
}  // namespace enrichers
}  // namespace logging
}  // namespace ws
