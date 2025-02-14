#pragma once

#include <sstream>
#ifdef _WIN32
#include <windows.h>
#define getpid() GetCurrentProcessId()
#else
#include <unistd.h>
#endif

#include "logging/events/log_event.h"
#include "logging/ilog_enricher.h"
namespace ws {
namespace logging {
namespace enrichers {

class ProcessIdEnricher : public ILogEnricher {
 public:
  void Enrich(ws::logging::events::LogEvent& event) const override;

 private:
  static const std::string kKey;
};
}  // namespace enrichers
}  // namespace logging
}  // namespace ws
