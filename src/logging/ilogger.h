#pragma once

#include <string>

#include "logging/log_level.h"
namespace ws {
namespace logging {

class ILogger {
 public:
  virtual void Log(LogLevel level, const std::string& message) const = 0;

  virtual void SetMinimumLogLevel(LogLevel level) = 0;

  virtual void LogVerbose(const std::string& message) const {
    Log(LogLevel::kVerbose, message);
  }

  virtual void LogInformation(const std::string& message) const {
    Log(LogLevel::kInformation, message);
  }

  virtual void LogWarning(const std::string& message) const {
    Log(LogLevel::kWarning, message);
  }

  virtual void LogError(const std::string& message) const {
    Log(LogLevel::kError, message);
  }

  virtual ~ILogger() = default;
};
}  // namespace logging
}  // namespace ws
