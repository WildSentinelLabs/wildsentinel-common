#include "logging/logger_configuration.h"

LoggerConfiguration& LoggerConfiguration::SetMinimumLogLevel(LogLevel level) {
  min_log_level_ = level;
  return *this;
}

LoggerConfiguration& LoggerConfiguration::AddSink(
    std::shared_ptr<ILogSink> sink, const bool async) {
  if (async)
    sink->EnableAsync();
  else
    sink->Enable();
  sinks_.push_back(std::move(sink));
  return *this;
}

LoggerConfiguration& LoggerConfiguration::AddConsoleSink(
    LogLevel min_log_level, const std::string& template_format) {
  auto console_sink = std::make_shared<ConsoleLogSink>(
      min_log_level, enrichers_, template_format);
  AddSink(console_sink);
  return *this;
}

LoggerConfiguration& LoggerConfiguration::AddEnricher(
    std::shared_ptr<ILogEnricher> enricher) {
  enrichers_.push_back(enricher);
  return *this;
}

std::shared_ptr<ILogger> LoggerConfiguration::CreateLogger(
    const std::string& source_context) {
  return std::make_shared<Logger>(source_context, sinks_, min_log_level_);
}
