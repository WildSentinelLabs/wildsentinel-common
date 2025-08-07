#pragma once

#include <thread>

#include "ws/logging/events/log_event.h"
#include "ws/logging/ilog_enricher.h"
#include "ws/string/format.h"
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

}  // namespace logging
}  // namespace ws
