#pragma once
#include <chrono>
#include <unordered_map>

#include "logging/log_level.h"
namespace ws {
namespace logging {
class LogEvent {
 public:
  LogEvent(const std::string& source_context, const std::string& message,
           LogLevel level = LogLevel::kInformation,
           const std::unordered_map<std::string, std::string>& properties =
               std::unordered_map<std::string, std::string>());

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
  std::unordered_map<std::string, std::string> properties_;
};
}  // namespace logging
}  // namespace ws
