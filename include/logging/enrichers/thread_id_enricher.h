#pragma once

#include <format>
#include <thread>

#include "logging/events/log_event.h"
#include "logging/ilog_enricher.h"
namespace ws {
namespace logging {
namespace enrichers {

class ThreadIdEnricher : public ILogEnricher {
 public:
  ThreadIdEnricher() = default;

  ~ThreadIdEnricher() override = default;

  void Enrich(ws::logging::events::LogEvent& event) const override;

 private:
  static const std::string kKey;
};
}  // namespace enrichers
}  // namespace logging
}  // namespace ws
