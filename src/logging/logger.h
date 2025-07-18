#pragma once

#include <atomic>
#include <future>
#include <thread>
#include <vector>

#include "logging/events/log_event.h"
#include "logging/ilog_enricher.h"
#include "logging/ilog_sink.h"
#include "logging/ilogger.h"
#include "logging/log_context.h"
#include "logging/log_level.h"
namespace ws {
namespace logging {
class LoggerConfiguration;
class Logger : public ILogger {
 public:
  explicit Logger(LoggerConfiguration& config,
                  const std::string& source_context = "");

  ~Logger() override = default;

  void Log(LogLevel level, const std::string& message) const override;

 private:
  LoggerConfiguration& config_;
  std::string source_context_;
};
}  // namespace logging
}  // namespace ws
