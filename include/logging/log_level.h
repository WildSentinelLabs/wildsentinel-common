#pragma once

#include <iostream>

enum class LogLevel {
  kVerbose = 0,
  kInformation = 1,
  kWarning = 2,
  kError = 3,
};

const std::string LogLevelToString(const LogLevel& log_level);

std::ostream& operator<<(std::ostream& os, const LogLevel& log_level);
