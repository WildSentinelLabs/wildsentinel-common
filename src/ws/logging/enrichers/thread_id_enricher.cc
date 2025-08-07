#include "ws/logging/enrichers/thread_id_enricher.h"

namespace ws {
namespace logging {

void ThreadIdEnricher::Enrich(ws::logging::LogEvent& event) const {
  event.AddProperty(kKey, GetCachedThreadId());
}

std::string ThreadIdEnricher::GetCachedThreadId() const {
  return Format("{}", std::this_thread::get_id());
}

}  // namespace logging
}  // namespace ws
