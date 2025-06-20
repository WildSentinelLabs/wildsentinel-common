#pragma once
#include <chrono>

#include "concurrency/collections/concurrent_unordered_map.h"
#include "logging/log_level.h"
namespace ws {
namespace logging {
class LogEvent {
 public:
  LogEvent(
      const std::string& source_context, const std::string& message,
      LogLevel level = LogLevel::kInformation,
      const ws::concurrency::ConcurrentUnorderedMap<std::string,
                                                    std::string>& properties =
          ws::concurrency::ConcurrentUnorderedMap<std::string, std::string>());

  std::string SourceContext() const;
  LogLevel Level() const;
  std::string Message() const;
  std::chrono::system_clock::time_point Timestamp() const;
  std::optional<std::string> GetProperty(const std::string& key) const;
  void AddProperty(const std::string& key, const std::string& value);

 private:
  std::string source_context_;
  LogLevel level_;
  std::string message_;
  std::chrono::system_clock::time_point timestamp_;
  ws::concurrency::ConcurrentUnorderedMap<std::string, std::string> properties_;
};

// ============================================================================
// Implementation details for LogEvent
// ============================================================================

inline std::string LogEvent::SourceContext() const { return source_context_; }

inline LogLevel LogEvent::Level() const { return level_; }

inline std::string LogEvent::Message() const { return message_; }

inline std::chrono::system_clock::time_point LogEvent::Timestamp() const {
  return timestamp_;
}
}  // namespace logging
}  // namespace ws
