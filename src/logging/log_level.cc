#include "logging/log_level.h"
namespace ws {
namespace logging {
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
}  // namespace logging
}  // namespace ws
