#include "logging/events/log_event.h"
namespace ws {
namespace logging {
LogEvent::LogEvent(
    const std::string& source_context, const std::string& message,
    LogLevel level,
    const std::unordered_map<std::string, std::string>& properties)
    : source_context_(source_context),
      level_(level),
      message_(message),
      properties_(properties) {
  timestamp_ = std::chrono::system_clock::now();
}

std::string LogEvent::SourceContext() const { return source_context_; }

LogLevel LogEvent::Level() const { return level_; }

std::string LogEvent::Message() const { return message_; }

std::chrono::system_clock::time_point LogEvent::Timestamp() const {
  return timestamp_;
}

std::optional<std::string> LogEvent::GetProperty(const std::string& key) const {
  auto it = properties_.find(key);
  if (it != properties_.end()) {
    std::string value = it->second;
    return value;
  }

  return std::nullopt;
}

void LogEvent::AddProperty(const std::string& key, const std::string& value) {
  properties_[key] = value;
}
}  // namespace logging
}  // namespace ws
