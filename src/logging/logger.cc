#include "logging/logger.h"

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

Logger::Logger(std::string source_context,
               std::vector<std::shared_ptr<ILogSink>> sinks,
               LogLevel min_log_level)
    : ILogger(),
      source_context_(source_context),
      sinks_(sinks),
      min_log_level_(min_log_level) {}

void Logger::Log(LogLevel level, const std::string& message) {
  if (level < min_log_level_) return;
  std::unordered_map<std::string, std::string> properties = properties_;
  LogEvent event(source_context_, message, level, properties);
  std::vector<std::future<void>> results;
  for (auto& sink : sinks_) {
    results.push_back(std::async(std::launch::async,
                                 [sink, event]() { sink->Display(event); }));
  }

  for (auto& result : results) {
    result.get();
  }
}

void Logger::SetMinimumLogLevel(LogLevel level) { min_log_level_ = level; }
