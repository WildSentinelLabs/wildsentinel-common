#include "ws/logging/rendering/message_renderer.h"

#include <stdexcept>
namespace ws {
namespace logging {
MessageRenderer::MessageRenderer(const std::string& template_format) {
  ParseTemplate(template_format);
}

std::string MessageRenderer::Render(const ws::logging::LogEvent& event) const {
  std::string result;
  for (const auto& part : template_parts_) {
    switch (part.type) {
      case TemplatePart::Type::Text:
        result += part.key;
        break;
      default:
        result += RenderPlaceholder(part, event);
        break;
    }
  }

  return result;
}

MessageRenderer::TemplatePart::TemplatePart(Type type, const std::string& key,
                                            const std::string& format)
    : type(type), key(key), format(format) {};

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
    const TemplatePart& part, const ws::logging::LogEvent& event) {
  if (part.key == kSourceContextKey) {
    return event.SourceContext();
  } else if (part.key == kMessageKey) {
    return event.Message();
  } else if (part.key == kLevelKey) {
    return FormatLogLevel(event.Level(), part.format);
  } else if (part.key == kTimeStampKey) {
    return FormatTimestamp(event.Timestamp(), part.format);
  } else {
    if (auto prop = event.GetProperty(part.key)) return *prop;
  }

  return "";
}

std::string MessageRenderer::FormatLogLevel(LogLevel level,
                                            const std::string& format) {
  if (format.empty()) {
    return LogLevelToString(level);
  }

  std::string formatter_type = format.substr(0, 1);
  int formatter_value = 0;
  if (format.length() > 1) {
    try {
      formatter_value = std::stoi(format.substr(1));
    } catch (const std::exception&) {
      return LogLevelToString(level);
    }
  }

  if (formatter_type == "u") {
    if (formatter_value == 3) {
      return LogLevelToString3Char(level);
    } else if (formatter_value > 0) {
      return LogLevelToString(level).substr(0, formatter_value);
    }
  } else if (formatter_type == "l") {
    std::string result;
    if (formatter_value == 3) {
      result = LogLevelToString3Char(level);
    } else if (formatter_value > 0) {
      result = LogLevelToString(level).substr(0, formatter_value);
    } else {
      result = LogLevelToString(level);
    }

    return ToLower(result);
  }

  return LogLevelToString(level);
}

std::string MessageRenderer::FormatTimestamp(
    std::chrono::system_clock::time_point timestamp,
    const std::string& format) {
  auto in_time_t = std::chrono::system_clock::to_time_t(timestamp);
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                timestamp.time_since_epoch()) %
            1000;
  std::tm localTime;
#ifdef _WIN32
  localtime_s(&localTime, &in_time_t);
#else
  localtime_r(&in_time_t, &localTime);
#endif

  char timeBuffer[128];
  std::string adjusted_format = format;

  bool include_ms = false;
  size_t pos = adjusted_format.find("%f");
  if (pos != std::string::npos) {
    include_ms = true;
    adjusted_format.erase(pos, 2);
  }

  std::strftime(timeBuffer, sizeof(timeBuffer), adjusted_format.c_str(),
                &localTime);
  std::string timeStr(timeBuffer);
  if (include_ms) return Format("{}{:03}", timeStr, ms.count());
  return timeStr;
}
}  // namespace logging
}  // namespace ws
