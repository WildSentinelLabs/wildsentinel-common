#include "logging/rendering/message_renderer.h"

MessageRenderer::MessageRenderer(const std::string& template_format) {
  ParseTemplate(template_format);
}

std::string MessageRenderer::Render(const LogEvent& event) const {
  std::ostringstream oss;
  for (const auto& part : template_parts_) {
    if (part.type == TemplatePart::Type::Text) {
      oss << part.key;
    } else {
      oss << RenderPlaceholder(part, event);
    }
  }

  return oss.str();
}

MessageRenderer::TemplatePart::TemplatePart(Type type, const std::string& key,
                                            const std::string& format)
    : type(type), key(key), format(format) {};

std::string MessageRenderer::source_context_key = "SourceContext";
std::string MessageRenderer::timestamp_key = "Timestamp";
std::string MessageRenderer::level_key = "Level";
std::string MessageRenderer::message_key = "Message";

void MessageRenderer::ParseTemplate(const std::string& format) {
  size_t last_pos = 0, pos;
  while ((pos = format.find('{', last_pos)) != std::string::npos) {
    if (pos > last_pos) {
      template_parts_.emplace_back(TemplatePart::Type::Text,
                                   format.substr(last_pos, pos - last_pos));
    }

    size_t end_pos = format.find('}', pos);
    if (end_pos == std::string::npos) break;

    std::string key_format = format.substr(pos + 1, end_pos - pos - 1);
    size_t colon_pos = key_format.find(':');
    std::string key = key_format.substr(0, colon_pos);
    std::string fmt =
        colon_pos != std::string::npos ? key_format.substr(colon_pos + 1) : "";

    template_parts_.emplace_back(TemplatePart::Type::Placeholder, key, fmt);
    last_pos = end_pos + 1;
  }

  if (last_pos < format.size()) {
    template_parts_.emplace_back(TemplatePart::Type::Text,
                                 format.substr(last_pos));
  }
}

std::string MessageRenderer::RenderPlaceholder(const TemplatePart& part,
                                               const LogEvent& event) {
  if (part.key == source_context_key) {
    return event.GetSourceContext();
  } else if (part.key == message_key) {
    return event.GetMessage();
  } else if (part.key == level_key) {
    return FormatLogLevel(event.GetLevel(), part.format);
  } else if (part.key == timestamp_key) {
    return FormatTimestamp(event.GetTimestamp(), part.format);
  } else {
    if (auto prop = event.GetProperty(part.key)) return *prop;
  }

  return "";
}

std::string MessageRenderer::FormatLogLevel(LogLevel level,
                                            const std::string& format) {
  std::string levelStr = LogLevelToString(level);
  if (format == "u3") {
    return levelStr.substr(0, 3);
  }
  return levelStr;
}

std::string MessageRenderer::FormatTimestamp(
    std::chrono::system_clock::time_point timestamp,
    const std::string& format) {
  auto in_time_t = std::chrono::system_clock::to_time_t(timestamp);
  std::ostringstream time_ss;
  time_ss << std::put_time(std::localtime(&in_time_t),
                           format.empty() ? "%Y-%m-%d %X" : format.c_str());
  return time_ss.str();
}
