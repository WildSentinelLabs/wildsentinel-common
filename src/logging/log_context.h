#pragma once
#include <atomic>

#include "logging/events/log_event.h"
#include "logging/log_level.h"

namespace ws {
namespace logging {
class Logger;
class LogContext {
 public:
  static void SetMinimumLogLevel(LogLevel level);

 private:
  friend class Logger;
  static std::atomic<LogLevel> min_log_level_;
  static LogEvent::map_type properties_;
};
// ============================================================================
// Implementation details for Logger
// ============================================================================

inline void LogContext::SetMinimumLogLevel(LogLevel level) {
  min_log_level_.exchange(level);
}

inline std::atomic<LogLevel> LogContext::min_log_level_ = LogLevel::kTrace;
inline LogEvent::map_type LogContext::properties_ = LogEvent::map_type();
}  // namespace logging
}  // namespace ws
