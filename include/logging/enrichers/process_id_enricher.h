#pragma once

#include "arch/cpu_arch.h"
#include "logging/events/log_event.h"
#include "logging/ilog_enricher.h"
namespace ws {
namespace logging {
namespace enrichers {

class ProcessIdEnricher : public ILogEnricher {
 public:
  ProcessIdEnricher() = default;

  ~ProcessIdEnricher() override = default;

  void Enrich(ws::logging::events::LogEvent& event) const override;

 private:
  static const std::string kKey;
};
}  // namespace enrichers
}  // namespace logging
}  // namespace ws
