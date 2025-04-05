#include "logging/logger_configuration.h"
namespace ws {
namespace logging {

LoggerConfiguration::LoggerConfiguration()
    : min_log_level_(LogLevel::kInformation) {}

LoggerConfiguration::LoggerConfiguration(LoggerConfiguration&& other) noexcept
    : sinks_(std::move(other.sinks_)),
      enrichers_(std::move(other.enrichers_)),
      min_log_level_(other.min_log_level_) {
  other.min_log_level_ = LogLevel::kInformation;
}

LoggerConfiguration& LoggerConfiguration::operator=(
    LoggerConfiguration&& other) noexcept {
  if (this != &other) {
    sinks_ = std::move(other.sinks_);
    enrichers_ = std::move(other.enrichers_);
    other.min_log_level_ = LogLevel::kInformation;
  }

  return *this;
}

LoggerConfiguration&& LoggerConfiguration::SetMinimumLogLevel(LogLevel level) {
  min_log_level_ = level;
  return std::move(*this);
}

LoggerConfiguration&& LoggerConfiguration::AddSink(
    std::unique_ptr<ILogSink>&& sink, bool enable_async) {
  if (enable_async)
    sink->EnableAsync();
  else
    sink->Enable();
  sinks_.push_back(std::move(sink));
  return std::move(*this);
}

LoggerConfiguration&& LoggerConfiguration::AddConsoleSink(
    LogLevel min_log_level, const std::string& template_format,
    bool enable_async) {
  std::unique_ptr<ILogSink> console_sink =
      std::make_unique<ws::logging::sinks::ConsoleLogSink>(min_log_level,
                                                           template_format);
  AddSink(std::move(console_sink), enable_async);
  return std::move(*this);
}

LoggerConfiguration&& LoggerConfiguration::AddEnricher(
    std::unique_ptr<ILogEnricher>&& enricher) {
  enrichers_.push_back(std::move(enricher));
  return std::move(*this);
}

std::unique_ptr<ILogger> LoggerConfiguration::CreateLogger(
    const std::string& source_context) {
  return std::unique_ptr<ILogger>(
      new Logger(source_context, min_log_level_, &sinks_, &enrichers_));
}
}  // namespace logging
}  // namespace ws
