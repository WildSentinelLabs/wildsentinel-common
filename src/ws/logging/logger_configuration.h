#pragma once

#include <memory>
#include <vector>

#include "ws/logging/ilog_enricher.h"
#include "ws/logging/logger.h"
#include "ws/logging/sinks/console_log_sink.h"

namespace ws {
namespace logging {
class LoggerConfiguration {
 public:
  LoggerConfiguration();
  LoggerConfiguration(const LoggerConfiguration&) = delete;
  LoggerConfiguration(LoggerConfiguration&&) noexcept;

  LoggerConfiguration& operator=(const LoggerConfiguration&) = delete;
  LoggerConfiguration& operator=(LoggerConfiguration&&) noexcept;

  LoggerConfiguration&& SetMinimumLogLevel(LogLevel level);
  LoggerConfiguration&& AddSink(std::unique_ptr<ILogSink>&& sink,
                                bool enable_async = false);
  LoggerConfiguration&& AddConsoleSink(
      LogLevel min_log_level = LogLevel::kInformation,
      const std::string& template_format =
          "{Timestamp:%Y-%m-%d %X} [{Level:u3}] "
          "{Message:lj}{NewLine}{Exception}",
      bool enable_async = false);
  LoggerConfiguration&& AddEnricher(std::unique_ptr<ILogEnricher>&& enricher);
  template <IsEnricher T>
  LoggerConfiguration&& AddEnricher();
  std::unique_ptr<Logger> Create(const std::string& source_context);
  template <typename T>
  std::unique_ptr<ILoggerOf<T>> Create();

 private:
  friend class Logger;

  LogLevel min_log_level_;
  std::vector<std::unique_ptr<ILogSink>> sinks_;
  std::vector<std::unique_ptr<ILogEnricher>> enrichers_;
};
// ============================================================================
// Implementation details for LoggerConfiguration
// ============================================================================

template <IsEnricher T>
inline LoggerConfiguration&& LoggerConfiguration::AddEnricher() {
  enrichers_.push_back(std::make_unique<T>());
  return std::move(*this);
}

template <typename T>
inline std::unique_ptr<ILoggerOf<T>> LoggerConfiguration::Create() {
  return std::make_unique<LoggerOf<T>>(*this);
}
}  // namespace logging
}  // namespace ws
