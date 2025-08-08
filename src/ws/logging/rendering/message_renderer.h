#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "ws/concurrency/concurrent_unordered_map.h"
#include "ws/logging/ilog_enricher.h"
#include "ws/logging/log_level.h"
#include "ws/string/format.h"
#include "ws/string/string_helpers.h"
namespace ws {
namespace logging {
class MessageRenderer {
 public:
  MessageRenderer(const std::string& template_format =
                      "{Timestamp:%Y-%m-%d %X} [{Level:u3}] "
                      "{Message:lj}{NewLine}{Exception}");

  std::string Render(const ws::logging::LogEvent& event) const;

 private:
  struct Placeholder {
    std::string key;
    std::string format;
  };

  static constexpr std::string_view kSourceContextKey = "SourceContext";
  static constexpr std::string_view kTimeStampKey = "Timestamp";
  static constexpr std::string_view kLevelKey = "Level";
  static constexpr std::string_view kMessageKey = "Message";
  static constexpr std::string_view kNewLineKey = "NewLine";
  static constexpr std::string_view kExceptionKey = "Exception";

  std::string ExtractValue(std::string_view key, const std::string& format,
                           const ws::logging::LogEvent& event) const;
  static std::string FormatLogLevel(LogLevel level, const std::string& format);
  static std::string FormatTimestamp(
      std::chrono::system_clock::time_point timestamp,
      const std::string& format);
  static std::string FormatNewLine();
  void ParseTemplate(const std::string& format);

  std::string template_format_;
  std::vector<Placeholder> placeholders_;
};
}  // namespace logging
}  // namespace ws
