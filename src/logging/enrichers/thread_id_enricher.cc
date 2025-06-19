#include "logging/enrichers/thread_id_enricher.h"

namespace ws {
namespace logging {

std::string GetCachedThreadId();

void ThreadIdEnricher::Enrich(ws::logging::LogEvent& event) const {
  event.AddProperty(kKey, GetCachedThreadId());
}

const std::string ThreadIdEnricher::kKey = "ThreadId";

std::string GetCachedThreadId() {
  return Format("{}", std::this_thread::get_id());
}

}  // namespace logging
}  // namespace ws
