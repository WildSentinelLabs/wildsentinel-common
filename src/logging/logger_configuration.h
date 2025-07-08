#pragma once

#include <memory>
#include <vector>

#include "logging/ilog_enricher.h"
#include "logging/ilogger.h"
#include "logging/logger.h"
#include "logging/sinks/console_log_sink.h"
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
  LoggerConfiguration&& AddEnricher() {
    enrichers_.push_back(std::make_unique<T>());
    return std::move(*this);
  }

  std::unique_ptr<ILogger> CreateLogger(const std::string& source_context);

 private:
  LogLevel min_log_level_;
  std::vector<std::unique_ptr<ILogSink>> sinks_;
  std::vector<std::unique_ptr<ILogEnricher>> enrichers_;
};
}  // namespace logging
}  // namespace ws
