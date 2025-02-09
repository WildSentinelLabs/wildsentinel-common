#pragma once

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "logging/ilog_enricher.h"
#include "logging/log_level.h"

class MessageRenderer {
 public:
  MessageRenderer(const std::string& template_format =
                      "{Timestamp:%Y-%m-%d %X} [{Level:u3}] "
                      "{Message:lj}{NewLine}{Exception}");

  std::string Render(const LogEvent& event) const;

 private:
  struct TemplatePart {
    enum class Type { Text, Placeholder };

    Type type;
    std::string key;
    std::string format;

    TemplatePart(Type type, const std::string& key = "",
                 const std::string& format = "");
  };

  static std::string source_context_key;
  static std::string timestamp_key;
  static std::string level_key;
  static std::string message_key;
  std::vector<TemplatePart> template_parts_;

  static std::string RenderPlaceholder(const TemplatePart& part,
                                       const LogEvent& event);

  static std::string FormatLogLevel(LogLevel level, const std::string& format);

  static std::string FormatTimestamp(
      std::chrono::system_clock::time_point timestamp,
      const std::string& format);

  void ParseTemplate(const std::string& format);
};

// TODO: Enhance template rendering and formating
