#pragma once

#include <chrono>
#include <format>
#include <iomanip>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "logging/ilog_enricher.h"
#include "logging/log_level.h"
namespace ws {
namespace logging {
namespace rendering {

class MessageRenderer {
 public:
  MessageRenderer(const std::string& template_format =
                      "{Timestamp:%Y-%m-%d %X} [{Level:u3}] "
                      "{Message:lj}{NewLine}{Exception}");

  std::string Render(const ws::logging::events::LogEvent& event) const;

 private:
  struct TemplatePart {
    enum class Type : int8_t { Text, Placeholder };

    Type type;
    std::string key;
    std::string format;

    TemplatePart(Type type, const std::string& key = "",
                 const std::string& format = "");
  };

  static const std::string kSourceContextKey;
  static const std::string kTimeStampKey;
  static const std::string kLevelKey;
  static const std::string kMessageKey;
  std::vector<TemplatePart> template_parts_;

  static std::string RenderPlaceholder(
      const TemplatePart& part, const ws::logging::events::LogEvent& event);

  static std::string FormatLogLevel(LogLevel level, const std::string& format);

  static std::string FormatTimestamp(
      std::chrono::system_clock::time_point timestamp,
      const std::string& format);

  void ParseTemplate(const std::string& format);
};

// TODO: Enhance template rendering and formating
}  // namespace rendering
}  // namespace logging
}  // namespace ws
