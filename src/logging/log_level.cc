#include "logging/log_level.h"
namespace ws {
namespace logging {
const std::string LogLevelToString(const LogLevel& log_level) {
  switch (log_level) {
    case LogLevel::kTrace:
      return "TRACE";
    case LogLevel::kDebug:
      return "DEBUG";
    case LogLevel::kInformation:
      return "INFORMATION";
    case LogLevel::kWarning:
      return "WARNING";
    case LogLevel::kError:
      return "ERROR";
    case LogLevel::kCritical:
      return "CRITICAL";
    default:
      return "UNKNOWN";
  }
}
}  // namespace logging
}  // namespace ws
