#include "ws/logging/events/log_event.h"
namespace ws {
namespace logging {
LogEvent::LogEvent(const std::string& source_context,
                   const std::string& message, LogLevel level,
                   const map_type& properties)
    : source_context_(source_context),
      level_(level),
      message_(message),
      properties_(properties) {
  timestamp_ = std::chrono::system_clock::now();
}

std::optional<LogEvent::mapped_type> LogEvent::GetProperty(
    const key_type& key) const {
  auto it = properties_.find(key);
  if (it != properties_.end()) return it->second;
  return std::nullopt;
}

void LogEvent::AddProperty(const key_type& key, const mapped_type& value) {
  properties_[key] = value;
}
}  // namespace logging
}  // namespace ws
