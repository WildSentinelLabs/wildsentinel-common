#include "logging/events/log_event.h"
namespace ws {
namespace logging {
LogEvent::LogEvent(
    const std::string& source_context, const std::string& message,
    LogLevel level,
    const ws::concurrency::ConcurrentUnorderedMap<std::string, std::string>&
        properties)
    : source_context_(source_context),
      level_(level),
      message_(message),
      properties_(properties) {
  timestamp_ = std::chrono::system_clock::now();
}

std::optional<std::string> LogEvent::GetProperty(const std::string& key) const {
  auto it = properties_.Find(key);
  if (it != properties_.end()) return it->second;
  return std::nullopt;
}

void LogEvent::AddProperty(const std::string& key, const std::string& value) {
  properties_[key] = value;
}
}  // namespace logging
}  // namespace ws
