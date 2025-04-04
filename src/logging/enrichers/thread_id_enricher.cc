#include "logging/enrichers/thread_id_enricher.h"
namespace ws {
namespace logging {
namespace enrichers {
void ThreadIdEnricher::Enrich(ws::logging::events::LogEvent& event) const {
  event.AddProperty(kKey, std::format("{}", std::this_thread::get_id()));
}

const std::string ThreadIdEnricher::kKey = "ThreadId";
}  // namespace enrichers
}  // namespace logging
}  // namespace ws
