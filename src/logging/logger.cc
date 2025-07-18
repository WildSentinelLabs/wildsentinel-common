#include "logging/logger.h"

#include "logging/logger_configuration.h"

namespace ws {
namespace logging {
Logger::Logger(LoggerConfiguration& config, const std::string& source_context,
               LogLevel min_log_level)
    : ILogger(),
      min_log_level_(min_log_level),
      source_context_(source_context),
      config_(config) {}

void Logger::Log(LogLevel level, const std::string& message) const {
  if (level < min_log_level_) return;
  LogEvent::map_type properties;
  ws::logging::LogEvent event(source_context_, message, level, properties);
  for (auto& enricher : config_.enrichers_) {
    enricher->Enrich(event);
  }

  std::vector<std::thread> sink_threads;
  for (auto& sink : config_.sinks_) {
    sink_threads.push_back(
        std::thread([sink = sink.get(), event]() { sink->Display(event); }));
  }

  for (auto& sink_thread : sink_threads) {
    sink_thread.join();
  }
}
}  // namespace logging
}  // namespace ws
