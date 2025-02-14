#pragma once
#include <chrono>
#include <unordered_map>

#include "logging/log_level.h"
namespace ws {
namespace logging {
namespace events {

class LogEvent {
 public:
  LogEvent(const std::string source_context, const std::string message,
           const LogLevel level = LogLevel::kInformation,
           const std::unordered_map<std::string, std::string> properties =
               std::unordered_map<std::string, std::string>());

  const std::string GetSourceContext() const;

  LogLevel GetLevel() const;

  const std::string GetMessage() const;

  std::chrono::system_clock::time_point GetTimestamp() const;

  std::optional<std::string> GetProperty(const std::string& key) const;

  void AddProperty(const std::string& key, const std::string& value);

 private:
  std::string source_context_;
  LogLevel level_;
  std::string message_;
  std::chrono::system_clock::time_point timestamp_;
  std::unordered_map<std::string, std::string> properties_;
};
}  // namespace events
}  // namespace logging
}  // namespace ws
