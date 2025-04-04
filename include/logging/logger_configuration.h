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
  LoggerConfiguration& SetMinimumLogLevel(LogLevel level);

  LoggerConfiguration& AddSink(std::shared_ptr<ILogSink> sink,
                               bool enable_async = false);

  LoggerConfiguration& AddConsoleSink(
      LogLevel min_log_level = LogLevel::kInformation,
      const std::string& template_format =
          "{Timestamp:%Y-%m-%d %X} [{Level:u3}] "
          "{Message:lj}{NewLine}{Exception}",
      bool enable_async = false);

  LoggerConfiguration& AddEnricher(std::shared_ptr<ILogEnricher> enricher);

  template <IsEnricher T>
  LoggerConfiguration& AddEnricher() {
    enrichers_.push_back(std::make_shared<T>());
    return *this;
  }

  std::unique_ptr<ILogger> CreateLogger(
      const std::string& source_context) const;

 private:
  LogLevel min_log_level_ = LogLevel::kInformation;
  std::vector<std::shared_ptr<ILogSink>> sinks_;
  std::vector<std::shared_ptr<ILogEnricher>> enrichers_;
};
}  // namespace logging
}  // namespace ws
