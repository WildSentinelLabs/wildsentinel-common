#pragma once

#include <iostream>
namespace ws {
namespace logging {

enum class LogLevel : int8_t {
  kVerbose = 0,
  kInformation = 1,
  kWarning = 2,
  kError = 3,
};

const std::string LogLevelToString(const LogLevel& log_level);
}  // namespace logging
}  // namespace ws
