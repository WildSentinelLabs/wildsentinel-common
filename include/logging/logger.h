#pragma once

#include <atomic>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
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
  static void AddProperty(const std::string& key, const std::string& value);

  static void RemoveProperty(const std::string& key);

  void Log(LogLevel level, const std::string& message) const override;

  void SetMinimumLogLevel(LogLevel level) override;

 private:
  friend class LoggerConfiguration;

  explicit Logger(
      const std::string& source_context, LogLevel min_log_level,
      std::vector<std::unique_ptr<ILogSink>>* sinks,
      std::vector<std::unique_ptr<ILogEnricher>>* enrichers = nullptr);

  std::string source_context_;
  std::vector<std::unique_ptr<ILogSink>>* sinks_;
  std::vector<std::unique_ptr<ILogEnricher>>* enrichers_;
  std::atomic<LogLevel> min_log_level_;
  static std::unordered_map<std::string, std::string> properties_;
  static std::mutex properties_mutex_;
};
}  // namespace logging
}  // namespace ws
