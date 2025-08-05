#pragma once

#include <string>

#include "ws/logging/log_level.h"
#include "ws/string/format.h"
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
  template <typename Arg, typename... Args>
  void Log(LogLevel level, const std::string& message, Arg&& arg,
           Args&&... args) const;
  template <typename Arg, typename... Args>
  void LogTrace(const std::string& message, Arg&& arg, Args&&... args) const;
  template <typename Arg, typename... Args>
  void LogDebug(const std::string& message, Arg&& arg, Args&&... args) const;
  template <typename Arg, typename... Args>
  void LogInformation(const std::string& message, Arg&& arg,
                      Args&&... args) const;
  template <typename Arg, typename... Args>
  void LogWarning(const std::string& message, Arg&& arg, Args&&... args) const;
  template <typename Arg, typename... Args>
  void LogError(const std::string& message, Arg&& arg, Args&&... args) const;
  template <typename Arg, typename... Args>
  void LogCritical(const std::string& message, Arg&& arg, Args&&... args) const;
};

template <typename T>
concept IsLoggerType = std::is_class_v<T>;

template <IsLoggerType T>
class ILoggerOf : public ILogger {};

// ============================================================================
// Implementation details for ILogger
// ============================================================================

template <typename Arg, typename... Args>
inline void ILogger::Log(LogLevel level, const std::string& message, Arg&& arg,
                         Args&&... args) const {
  Log(level,
      Format(message, std::forward<Arg>(arg), std::forward<Args>(args)...));
}

template <typename Arg, typename... Args>
inline void ILogger::LogTrace(const std::string& message, Arg&& arg,
                              Args&&... args) const {
  Log(LogLevel::kTrace,
      Format(message, std::forward<Arg>(arg), std::forward<Args>(args)...));
}

template <typename Arg, typename... Args>
inline void ILogger::LogDebug(const std::string& message, Arg&& arg,
                              Args&&... args) const {
  Log(LogLevel::kDebug,
      Format(message, std::forward<Arg>(arg), std::forward<Args>(args)...));
}

template <typename Arg, typename... Args>
inline void ILogger::LogInformation(const std::string& message, Arg&& arg,
                                    Args&&... args) const {
  Log(LogLevel::kInformation,
      Format(message, std::forward<Arg>(arg), std::forward<Args>(args)...));
}

template <typename Arg, typename... Args>
inline void ILogger::LogWarning(const std::string& message, Arg&& arg,
                                Args&&... args) const {
  Log(LogLevel::kWarning,
      Format(message, std::forward<Arg>(arg), std::forward<Args>(args)...));
}

template <typename Arg, typename... Args>
inline void ILogger::LogError(const std::string& message, Arg&& arg,
                              Args&&... args) const {
  Log(LogLevel::kError,
      Format(message, std::forward<Arg>(arg), std::forward<Args>(args)...));
}

template <typename Arg, typename... Args>
inline void ILogger::LogCritical(const std::string& message, Arg&& arg,
                                 Args&&... args) const {
  Log(LogLevel::kCritical,
      Format(message, std::forward<Arg>(arg), std::forward<Args>(args)...));
}
}  // namespace logging
}  // namespace ws
