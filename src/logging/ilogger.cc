
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

template <typename... Args>
void ILogger::Log(LogLevel level, const std::string& message,
                  Args&&... args) const {
  Log(level, Format(message, std::forward<Args>(args)...));
}

template <typename... Args>
void ILogger::LogTrace(const std::string& message, Args&&... args) const {
  LogTrace(Format(message, std::forward<Args>(args)...));
}

template <typename... Args>
void ILogger::LogDebug(const std::string& message, Args&&... args) const {
  LogDebug(Format(message, std::forward<Args>(args)...));
}

template <typename... Args>
void ILogger::LogInformation(const std::string& message, Args&&... args) const {
  LogInformation(Format(message, std::forward<Args>(args)...));
}

template <typename... Args>
void ILogger::LogWarning(const std::string& message, Args&&... args) const {
  LogWarning(Format(message, std::forward<Args>(args)...));
}

template <typename... Args>
void ILogger::LogError(const std::string& message, Args&&... args) const {
  LogError(Format(message, std::forward<Args>(args)...));
}

template <typename... Args>
void ILogger::LogCritical(const std::string& message, Args&&... args) const {
  LogCritical(Format(message, std::forward<Args>(args)...));
}

}  // namespace logging
}  // namespace ws
