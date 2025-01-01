#pragma once
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

class Logger {
 public:
  explicit Logger(const std::string context);

  ~Logger() = default;

  void LogInfomation(const std::string& message) const;

  void LogVerbose(const std::string& message) const;

  void LogWarning(const std::string& message) const;

  void LogError(const std::string& message) const;

 private:
  const std::string context_;
  mutable std::mutex mutex_;

  std::string CurrentDateTime() const;

  enum class Level {
    kINFO,
    kERR,
    kVERBOSE,
    kWARNING,
    kERROR,
  };

  void Log(Level level, const std::string& message) const;

  std::string FormatLogMessage(Level level, const std::string& message) const;

  std::string LevelToString(Level level) const;
};
