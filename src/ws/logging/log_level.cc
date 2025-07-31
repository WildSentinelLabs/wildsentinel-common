#include "ws/logging/log_level.h"
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

const std::string LogLevelToString3Char(const LogLevel& log_level) {
  switch (log_level) {
    case LogLevel::kTrace:
      return "TRC";
    case LogLevel::kDebug:
      return "DBG";
    case LogLevel::kInformation:
      return "INF";
    case LogLevel::kWarning:
      return "WRN";
    case LogLevel::kError:
      return "ERR";
    case LogLevel::kCritical:
      return "CRT";
    default:
      return "UNK";
  }
}

}  // namespace logging
}  // namespace ws
