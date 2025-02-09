#pragma once

#include <memory>

#include "logging/ilog_enricher.h"
#include "logging/ilogger.h"
#include "logging/logger.h"
#include "logging/sinks/console_log_sink.h"

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

  std::shared_ptr<ILogger> CreateLogger(const std::string& source_context);

 private:
  LogLevel min_log_level_ = LogLevel::kInformation;
  std::vector<std::shared_ptr<ILogSink>> sinks_;
  std::vector<std::shared_ptr<ILogEnricher>> enrichers_;
};
