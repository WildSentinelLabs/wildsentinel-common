
#include "logging/ilogger.h"
namespace ws {
namespace logging {

void ILogger::LogTrace(const std::string& message) const {
  Log(LogLevel::kTrace, message);
}

void ILogger::LogDebug(const std::string& message) const {
  Log(LogLevel::kDebug, message);
}

void ILogger::LogInformation(const std::string& message) const {
  Log(LogLevel::kInformation, message);
}

void ILogger::LogWarning(const std::string& message) const {
  Log(LogLevel::kWarning, message);
}

void ILogger::LogError(const std::string& message) const {
  Log(LogLevel::kError, message);
}

void ILogger::LogCritical(const std::string& message) const {
  Log(LogLevel::kCritical, message);
}
}  // namespace logging
}  // namespace ws
