#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <thread>

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

  static std::queue<std::string> log_queue_;
  static std::mutex queue_mutex_;
  static std::condition_variable cv_;
  static std::thread log_thread_;
  static std::atomic<bool> running_;
  static std::atomic<bool> done_logging_;

  void Log(LogLevel level, const std::string& message);
  std::string CurrentDateTime() const;
  std::string FormatLogMessage(LogLevel level,
                               const std::string& message) const;
  std::string LogLevelToString(LogLevel level) const;

  static void ProcessLogs();
};
