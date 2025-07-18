#pragma once
#include <cstdint>
#include <string>
namespace ws {
namespace logging {
enum class LogLevel : int8_t {
  kTrace = 0,
  kDebug = 1,
  kInformation = 2,
  kWarning = 3,
  kError = 4,
  kCritical = 5,
};

const std::string LogLevelToString(const LogLevel& log_level);
}  // namespace logging
}  // namespace ws
