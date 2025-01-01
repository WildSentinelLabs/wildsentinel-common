#include "logging/logger.h"

std::string Logger::CurrentDateTime() const {
  auto now = std::chrono::system_clock::now();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) %
            1000;
  std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
  std::tm tm = *std::localtime(&now_time_t);
  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << '.' << std::setw(3)
      << std::setfill('0') << ms.count();
  return oss.str();
}

void Logger::Log(Level level, const std::string& message) const {
  std::lock_guard<std::mutex> lock(mutex_);
  std::string formattedMessage = FormatLogMessage(level, message);
  std::cout << formattedMessage << std::endl;
}

std::string Logger::FormatLogMessage(Level level,
                                     const std::string& message) const {
  std::ostringstream oss;
  oss << "[" << CurrentDateTime() << "] [" << context_ << "] ["
      << LevelToString(level) << "] " << message;
  return oss.str();
}

std::string Logger::LevelToString(Level level) const {
  switch (level) {
    case Level::kINFO:
      return "INFO";
    case Level::kVERBOSE:
      return "VERBOSE";
    case Level::kWARNING:
      return "WARNING";
    case Level::kERROR:
      return "ERROR";
    default:
      return "UNKNOWN";
  }
}

Logger::Logger(const std::string context) : context_(context) {};

void Logger::LogInfomation(const std::string& message) const {
  Log(Level::kINFO, message);
}

void Logger::LogVerbose(const std::string& message) const {
  Log(Level::kVERBOSE, message);
}

void Logger::LogWarning(const std::string& message) const {
  Log(Level::kWARNING, message);
}

void Logger::LogError(const std::string& message) const {
  Log(Level::kERROR, message);
}
