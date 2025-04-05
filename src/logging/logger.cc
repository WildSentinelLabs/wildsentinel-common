#include "logging/logger.h"
namespace ws {
namespace logging {

std::unordered_map<std::string, std::string> Logger::properties_;
std::mutex Logger::properties_mutex_;

void Logger::AddProperty(const std::string& key, const std::string& value) {
  std::lock_guard<std::mutex> lock(properties_mutex_);
  properties_[key] = value;
}

void Logger::RemoveProperty(const std::string& key) {
  std::lock_guard<std::mutex> lock(properties_mutex_);
  properties_.erase(key);
}

Logger::Logger(const std::string& source_context, LogLevel min_log_level,
               std::vector<std::unique_ptr<ILogSink>>* sinks,
               std::vector<std::unique_ptr<ILogEnricher>>* enrichers)
    : ILogger(),
      sinks_(sinks),
      source_context_(source_context),
      min_log_level_(min_log_level),
      enrichers_(enrichers) {}

void Logger::Log(LogLevel level, const std::string& message) const {
  if (level < min_log_level_) return;
  std::unordered_map<std::string, std::string> properties = properties_;
  ws::logging::events::LogEvent event(source_context_, message, level,
                                      properties);
  for (auto& enricher : *enrichers_) enricher->Enrich(event);
  std::vector<std::future<void>> results;
  for (auto& sink : *sinks_) {
    results.push_back(std::async(std::launch::async,
                                 [&sink, event]() { sink->Display(event); }));
  }

  for (auto& result : results) {
    result.get();
  }
}

void Logger::SetMinimumLogLevel(LogLevel level) { min_log_level_ = level; }
}  // namespace logging
}  // namespace ws
