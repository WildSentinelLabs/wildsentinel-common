#pragma once

#include <thread>

#include "format.h"
#include "logging/events/log_event.h"
#include "logging/ilog_enricher.h"
namespace ws {
namespace logging {

class ThreadIdEnricher : public ILogEnricher {
 public:
  ThreadIdEnricher() = default;

  ~ThreadIdEnricher() override = default;

  void Enrich(ws::logging::LogEvent& event) const override;

 private:
  static const std::string kKey;

  std::string GetCachedThreadId() const;
};

// ============================================================================
// Implementation details for ThreadIdEnricher
// ============================================================================

inline void ThreadIdEnricher::Enrich(ws::logging::LogEvent& event) const {
  event.AddProperty(kKey, GetCachedThreadId());
}

inline const std::string ThreadIdEnricher::kKey = "ThreadId";

inline std::string ThreadIdEnricher::GetCachedThreadId() const {
  return Format("{}", std::this_thread::get_id());
}

}  // namespace logging
}  // namespace ws
