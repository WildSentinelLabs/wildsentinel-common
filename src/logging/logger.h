#pragma once

#include <atomic>
#include <future>
#include <thread>
#include <vector>

#include "logging/events/log_event.h"
#include "logging/ilog_enricher.h"
#include "logging/ilog_sink.h"
#include "logging/ilogger.h"
#include "logging/log_level.h"
namespace ws {
namespace logging {
class LoggerConfiguration;
class Logger : public ILogger {
 public:
  ~Logger() override = default;

  void Log(LogLevel level, const std::string& message) const override;
  void SetMinimumLogLevel(LogLevel level) override;

  explicit Logger(LoggerConfiguration& config,
                  const std::string& source_context = "",
                  LogLevel min_log_level = LogLevel::kInformation);

 private:
  LoggerConfiguration& config_;
  std::string source_context_;
  std::atomic<LogLevel> min_log_level_;
};

// ============================================================================
// Implementation details for Logger
// ============================================================================

inline void Logger::SetMinimumLogLevel(LogLevel level) {
  min_log_level_.exchange(level);
}

}  // namespace logging
}  // namespace ws
