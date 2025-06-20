#include "logging/logger.h"
namespace ws {
namespace logging {
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
  ws::concurrency::ConcurrentUnorderedMap<std::string, std::string> properties;
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
}  // namespace logging
}  // namespace ws
