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
      min_log_level_(min_log_level),
      source_context_(source_context),
      sinks_(sinks),
      enrichers_(enrichers) {}

void Logger::Log(LogLevel level, const std::string& message) const {
  if (level < min_log_level_) return;
  std::unordered_map<std::string, std::string> properties = properties_;
  ws::logging::LogEvent event(source_context_, message, level, properties);
  for (auto& enricher : *enrichers_) {
    enricher->Enrich(event);
  }

  std::vector<std::thread> sink_threads;
  for (auto& sink : *sinks_) {
    sink_threads.push_back(
        std::thread([sink = sink.get(), event]() { sink->Display(event); }));
  }

  for (auto& sink_thread : sink_threads) {
    sink_thread.join();
  }
}

void Logger::SetMinimumLogLevel(LogLevel level) { min_log_level_ = level; }
}  // namespace logging
}  // namespace ws
