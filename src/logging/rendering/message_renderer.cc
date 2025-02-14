#include "logging/rendering/message_renderer.h"
namespace ws {
namespace logging {
namespace rendering {
MessageRenderer::MessageRenderer(const std::string& template_format) {
  ParseTemplate(template_format);
}

std::string MessageRenderer::Render(
    const ws::logging::events::LogEvent& event) const {
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

const std::string MessageRenderer::kSourceContextKey = "SourceContext";
const std::string MessageRenderer::kTimeStampKey = "Timestamp";
const std::string MessageRenderer::kLevelKey = "Level";
const std::string MessageRenderer::kMessageKey = "Message";

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

std::string MessageRenderer::RenderPlaceholder(
    const TemplatePart& part, const ws::logging::events::LogEvent& event) {
  if (part.key == kSourceContextKey) {
    return event.GetSourceContext();
  } else if (part.key == kMessageKey) {
    return event.GetMessage();
  } else if (part.key == kLevelKey) {
    return FormatLogLevel(event.GetLevel(), part.format);
  } else if (part.key == kTimeStampKey) {
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
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                timestamp.time_since_epoch()) %
            1000;

  std::ostringstream time_ss;
  std::tm localTime;
#ifdef _WIN32
  localtime_s(&localTime, &in_time_t);
#else
  localtime_r(&in_time_t, &localTime);
#endif

  std::string adjusted_format = format;
  bool include_ms = (format.find("%f") != std::string::npos);
  if (include_ms) {
    adjusted_format = format;
    adjusted_format.erase(adjusted_format.find("%f"), 2);
  }

  time_ss << std::put_time(&localTime, adjusted_format.c_str());
  if (include_ms) {
    time_ss << std::setfill('0') << std::setw(3) << ms.count();
  }

  return time_ss.str();
}
}  // namespace rendering
}  // namespace logging
}  // namespace ws
