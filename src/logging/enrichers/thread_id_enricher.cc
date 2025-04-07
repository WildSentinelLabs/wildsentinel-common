#include "logging/enrichers/thread_id_enricher.h"

namespace ws {
namespace logging {
namespace enrichers {
std::string GetCachedThreadId();

void ThreadIdEnricher::Enrich(ws::logging::events::LogEvent& event) const {
  event.AddProperty(kKey, GetCachedThreadId());
}

const std::string ThreadIdEnricher::kKey = "ThreadId";

std::string GetCachedThreadId() {
  std::ostringstream oss;
  oss << std::this_thread::get_id();
  return oss.str();
}
}  // namespace enrichers
}  // namespace logging
}  // namespace ws
