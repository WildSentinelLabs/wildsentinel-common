#pragma once
#include <chrono>
#include <optional>

#include "ws/concurrency/concurrent_unordered_map.h"
#include "ws/logging/log_level.h"
namespace ws {
namespace logging {
class LogEvent {
 public:
  using key_type = std::string;
  using mapped_type = std::string;
  using map_type = std::concurrent_unordered_map<key_type, mapped_type>;

  LogEvent(const std::string& source_context, const std::string& message,
           LogLevel level = LogLevel::kInformation,
           const map_type& properties = map_type());

  std::string SourceContext() const;
  constexpr LogLevel Level() const;
  std::string Message() const;
  std::chrono::system_clock::time_point Timestamp() const;
  std::optional<mapped_type> GetProperty(const key_type& key) const;
  void AddProperty(const key_type& key, const mapped_type& value);

 private:
  std::string source_context_;
  LogLevel level_;
  std::string message_;
  std::chrono::system_clock::time_point timestamp_;
  map_type properties_;
};

// ============================================================================
// Implementation details for LogEvent
// ============================================================================

inline std::string LogEvent::SourceContext() const { return source_context_; }

inline constexpr LogLevel LogEvent::Level() const { return level_; }

inline std::string LogEvent::Message() const { return message_; }

inline std::chrono::system_clock::time_point LogEvent::Timestamp() const {
  return timestamp_;
}
}  // namespace logging
}  // namespace ws
