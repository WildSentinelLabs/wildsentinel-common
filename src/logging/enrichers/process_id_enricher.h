#pragma once

#include "arch/cpu_arch.h"
#include "format.h"
#include "logging/events/log_event.h"
#include "logging/ilog_enricher.h"
namespace ws {
namespace logging {

class ProcessIdEnricher : public ILogEnricher {
 public:
  ProcessIdEnricher() = default;

  ~ProcessIdEnricher() override = default;

  void Enrich(ws::logging::LogEvent& event) const override;

 private:
  static const std::string kKey;
};

}  // namespace logging
}  // namespace ws
