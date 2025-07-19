#pragma once

#include <string>

#include "logging/log_level.h"
#include "string/format.h"
namespace ws {
namespace logging {
class ILogger {
 public:
  virtual ~ILogger() = default;

  virtual void Log(LogLevel level, const std::string& message) const = 0;
  void LogTrace(const std::string& message) const;
  void LogDebug(const std::string& message) const;
  void LogInformation(const std::string& message) const;
  void LogWarning(const std::string& message) const;
  void LogError(const std::string& message) const;
  void LogCritical(const std::string& message) const;
  template <typename... Args>
  void Log(LogLevel level, const std::string& message, Args&&... args) const;
  template <typename... Args>
  void LogTrace(const std::string& message, Args&&... args) const;
  template <typename... Args>
  void LogDebug(const std::string& message, Args&&... args) const;
  template <typename... Args>
  void LogInformation(const std::string& message, Args&&... args) const;
  template <typename... Args>
  void LogWarning(const std::string& message, Args&&... args) const;
  template <typename... Args>
  void LogError(const std::string& message, Args&&... args) const;
  template <typename... Args>
  void LogCritical(const std::string& message, Args&&... args) const;
};
}  // namespace logging
}  // namespace ws
