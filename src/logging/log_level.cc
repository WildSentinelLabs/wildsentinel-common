#include "logging/log_level.h"

const std::string LogLevelToString(const LogLevel& log_level) {
  switch (log_level) {
    case LogLevel::kVerbose:
      return "VERBOSE";
    case LogLevel::kInformation:
      return "INFORMATION";
    case LogLevel::kWarning:
      return "WARNING";
    case LogLevel::kError:
      return "ERROR";
    default:
      return "UNKNOWN";
  }
}

std::ostream& operator<<(std::ostream& os, const LogLevel& log_level) {
  os << LogLevelToString(log_level);
  return os;
}
