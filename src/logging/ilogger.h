#pragma once

#include <string>

#include "logging/log_level.h"
namespace ws {
namespace logging {
class ILogger {
 public:
  virtual ~ILogger() = default;

  virtual void Log(LogLevel level, const std::string& message) const = 0;
  virtual void SetMinimumLogLevel(LogLevel level) = 0;
  virtual void LogTrace(const std::string& message) const;
  virtual void LogDebug(const std::string& message) const;
  virtual void LogInformation(const std::string& message) const;
  virtual void LogWarning(const std::string& message) const;
  virtual void LogError(const std::string& message) const;
  virtual void LogCritical(const std::string& message) const;
};
}  // namespace logging
}  // namespace ws
