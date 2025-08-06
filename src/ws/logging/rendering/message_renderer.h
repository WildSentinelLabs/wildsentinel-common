#pragma once

#include <chrono>
#include <iomanip>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

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
  struct TemplatePart {
    enum class Type : int8_t { Text, Placeholder };

    Type type;
    std::string key;
    std::string format;

    TemplatePart(Type type, const std::string& key = "",
                 const std::string& format = "");
  };

  static constexpr std::string_view kSourceContextKey = "SourceContext";
  static constexpr std::string_view kTimeStampKey = "Timestamp";
  static constexpr std::string_view kLevelKey = "Level";
  static constexpr std::string_view kMessageKey = "Message";
  static constexpr std::string_view kNewLineKey = "NewLine";

  static std::string RenderPlaceholder(const TemplatePart& part,
                                       const ws::logging::LogEvent& event);
  static std::string FormatLogLevel(LogLevel level, const std::string& format);
  static std::string FormatTimestamp(
      std::chrono::system_clock::time_point timestamp,
      const std::string& format);
  static std::string FormatNewLine(const std::string& format);
  void ParseTemplate(const std::string& format);

  std::vector<TemplatePart> template_parts_;
};
// TODO: Enhance template rendering and formating
}  // namespace logging
}  // namespace ws
