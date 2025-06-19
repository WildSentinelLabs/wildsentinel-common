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
};

}  // namespace logging
}  // namespace ws
