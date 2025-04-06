#include "logging/enrichers/thread_id_enricher.h"

namespace ws {
namespace logging {
namespace enrichers {
void ThreadIdEnricher::Enrich(ws::logging::events::LogEvent& event) const {
  thread_local std::string cachedThreadId = []() {
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    return oss.str();
  }();

  event.AddProperty(kKey, cachedThreadId);
}

const std::string ThreadIdEnricher::kKey = "ThreadId";
}  // namespace enrichers
}  // namespace logging
}  // namespace ws
