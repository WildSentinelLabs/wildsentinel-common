#include "ws/logging/logger.h"

#include "ws/logging/logger_configuration.h"

namespace ws {
namespace logging {
Logger::Logger(LoggerConfiguration& config, const std::string& source_context)
    : ILogger(), source_context_(source_context), config_(config) {}

void Logger::Log(LogLevel level, const std::string& message) const {
  if (level < LogContext::min_log_level_) return;
  if (level < config_.min_log_level_) return;
  LogEvent::map_type properties;
  ws::logging::LogEvent event(source_context_, message, level, properties);
  for (auto& enricher : config_.enrichers_) {
    enricher->Enrich(event);
  }

  for (auto& sink : config_.sinks_) {
    sink->Display(event);
  }
}
}  // namespace logging
}  // namespace ws
