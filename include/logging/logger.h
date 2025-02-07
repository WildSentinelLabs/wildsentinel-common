#pragma once

#include <atomic>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include "collections/concurrent/concurrent_queue.h"

enum class LogLevel {
  kVerbose,
  kInfo,
  kWarning,
  kError,
};

class Logger {
 public:
  explicit Logger(const std::string& context);
  ~Logger();

  void LogVerbose(const std::string& message);
  void LogInformation(const std::string& message);
  void LogWarning(const std::string& message);
  void LogError(const std::string& message);

 private:
  std::string context_;

  static ConcurrentQueue<std::string> log_queue_;
  static std::thread log_thread_;
  static std::atomic<bool> running_;

  void Log(LogLevel level, const std::string& message);
  std::string CurrentDateTime() const;
  std::string FormatLogMessage(LogLevel level,
                               const std::string& message) const;
  std::string LogLevelToString(LogLevel level) const;

  static void ProcessLogs();
};
