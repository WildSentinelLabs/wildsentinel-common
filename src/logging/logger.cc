#include "logging/logger.h"

std::queue<std::string> Logger::log_queue_;
std::mutex Logger::queue_mutex_;
std::condition_variable Logger::cv_;
std::thread Logger::log_thread_;
std::atomic<bool> Logger::running_(false);
std::atomic<bool> Logger::done_logging_(false);

Logger::Logger(const std::string& context) : context_(context) {
  if (!running_) {
    running_ = true;
    done_logging_ = false;
    log_thread_ = std::thread(&Logger::ProcessLogs);
  }
}

Logger::~Logger() {
  running_ = false;
  cv_.notify_all();

  {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    cv_.wait(lock, [] { return log_queue_.empty(); });
  }

  done_logging_ = true;
  if (log_thread_.joinable()) {
    log_thread_.join();
  }
}

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

std::string Logger::FormatLogMessage(LogLevel level,
                                     const std::string& message) const {
  std::ostringstream oss;
  oss << "[" << CurrentDateTime() << "] [" << context_ << "] ["
      << LogLevelToString(level) << "] " << message;
  return oss.str();
}

std::string Logger::LogLevelToString(LogLevel level) const {
  switch (level) {
    case LogLevel::kVerbose:
      return "VERBOSE";
    case LogLevel::kInfo:
      return "INFO";
    case LogLevel::kWarning:
      return "WARNING";
    case LogLevel::kError:
      return "ERROR";
    default:
      return "UNKNOWN";
  }
}

void Logger::Log(LogLevel level, const std::string& message) {
  std::string formatted_message = FormatLogMessage(level, message);

  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    log_queue_.push(formatted_message);
  }

  cv_.notify_one();
}

void Logger::LogVerbose(const std::string& message) {
  Log(LogLevel::kVerbose, message);
}
void Logger::LogInformation(const std::string& message) {
  Log(LogLevel::kInfo, message);
}
void Logger::LogWarning(const std::string& message) {
  Log(LogLevel::kWarning, message);
}
void Logger::LogError(const std::string& message) {
  Log(LogLevel::kError, message);
}

void Logger::ProcessLogs() {
  while (running_ || !log_queue_.empty()) {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    cv_.wait(lock, [] { return !log_queue_.empty() || !running_; });

    while (!log_queue_.empty()) {
      std::cout << log_queue_.front() << std::endl;
      log_queue_.pop();
    }
  }

  done_logging_ = true;
  cv_.notify_all();
}
