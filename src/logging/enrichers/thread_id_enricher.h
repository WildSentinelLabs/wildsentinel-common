#pragma once

#include <thread>

#include "logging/events/log_event.h"
#include "logging/ilog_enricher.h"
#include "string/format.h"
namespace ws {
namespace logging {
class ThreadIdEnricher : public ILogEnricher {
 public:
  ThreadIdEnricher() = default;

  ~ThreadIdEnricher() override = default;

  void Enrich(ws::logging::LogEvent& event) const override;

 private:
  static constexpr std::string_view kKey = "ThreadId";

  std::string GetCachedThreadId() const;
};

// ============================================================================
// Implementation details for ThreadIdEnricher
// ============================================================================

inline void ThreadIdEnricher::Enrich(ws::logging::LogEvent& event) const {
  event.AddProperty(std::string(kKey), GetCachedThreadId());
}

inline std::string ThreadIdEnricher::GetCachedThreadId() const {
  return Format("{}", std::this_thread::get_id());
}
}  // namespace logging
}  // namespace ws
