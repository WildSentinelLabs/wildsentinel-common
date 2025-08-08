#pragma once

#define KEEP_WS_ORDER
#include "ws/system.h"
#undef KEEP_WS_ORDER

#include "ws/logging/events/log_event.h"
#include "ws/logging/ilog_enricher.h"
#include "ws/string/format.h"
namespace ws {
namespace logging {

class ProcessIdEnricher : public ILogEnricher {
 public:
  ProcessIdEnricher() = default;

  ~ProcessIdEnricher() override = default;

  void Enrich(ws::logging::LogEvent& event) const override;

 private:
  static constexpr std::string_view kKey = "ProcessId";
};

}  // namespace logging
}  // namespace ws
