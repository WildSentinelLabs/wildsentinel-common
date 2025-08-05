#pragma once

#include <atomic>
#include <future>
#include <thread>
#include <vector>

#include "ws/logging/events/log_event.h"
#include "ws/logging/ilog_enricher.h"
#include "ws/logging/ilog_sink.h"
#include "ws/logging/ilogger.h"
#include "ws/logging/log_context.h"
#include "ws/logging/log_level.h"
namespace ws {
namespace logging {
class LoggerConfiguration;
class Logger : public ILogger {
 public:
  explicit Logger(LoggerConfiguration& config,
                  const std::string& source_context = "");

  ~Logger() override = default;

  void Log(LogLevel level, const std::string& message) const override;

 private:
  LoggerConfiguration& config_;
  std::string source_context_;
};

#define EXTRACT_CLASS_NAME(T)                   \
  []() constexpr -> const char* {               \
    constexpr const char* full_name = #T;       \
    const char* last_colon = nullptr;           \
    for (const char* p = full_name; *p; ++p) {  \
      if (*p == ':' && *(p + 1) == ':') {       \
        last_colon = p + 2;                     \
      }                                         \
    }                                           \
    return last_colon ? last_colon : full_name; \
  }()

template <typename T>
class LoggerOf : public Logger {
 public:
  explicit LoggerOf(LoggerConfiguration& config);

  ~LoggerOf() override = default;
};
// ============================================================================
// Implementation details for Logger
// ============================================================================

template <typename T>
inline LoggerOf<T>::LoggerOf(LoggerConfiguration& config)
    : Logger(config, EXTRACT_CLASS_NAME(T)) {}

}  // namespace logging
}  // namespace ws
