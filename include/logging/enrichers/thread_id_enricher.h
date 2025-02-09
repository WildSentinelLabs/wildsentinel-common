#pragma once

#include <sstream>
#include <thread>

#include "logging/events/log_event.h"
#include "logging/ilog_enricher.h"

class ThreadIdEnricher : public ILogEnricher {
 public:
  void Enrich(LogEvent& event) const override;

 private:
  std::string key_ = "ThreadId";
};
