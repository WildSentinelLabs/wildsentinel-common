#include "ws/logging/rendering/message_renderer.h"

namespace ws {
namespace logging {
MessageRenderer::MessageRenderer(const std::string& template_format) {
  ParseTemplate(template_format);
}

std::string MessageRenderer::Render(const ws::logging::LogEvent& event) const {
  std::string result = template_format_;
  for (const auto& placeholder : placeholders_) {
    std::string value =
        ExtractValue(placeholder.key, placeholder.format, event);

    size_t pos = result.find("{}");
    if (pos != std::string::npos) {
      result.replace(pos, 2, value);
    }
  }

  return result;
}

void MessageRenderer::ParseTemplate(const std::string& format) {
  size_t last_pos = 0, pos = 0;
  placeholders_.clear();
  template_format_.clear();
  while ((pos = format.find('{', last_pos)) != std::string::npos) {
    if (pos > last_pos)
      template_format_ += format.substr(last_pos, pos - last_pos);

    size_t end_pos = format.find('}', pos);
    if (end_pos == std::string::npos) break;
    std::string key_format = format.substr(pos + 1, end_pos - pos - 1);
    size_t colon_pos = key_format.find(':');
    std::string key = key_format.substr(0, colon_pos);
    std::string fmt =
        colon_pos != std::string::npos ? key_format.substr(colon_pos + 1) : "";

    template_format_ += "{}";
    placeholders_.emplace_back(key, fmt);
    last_pos = end_pos + 1;
  }

  if (last_pos < format.size()) template_format_ += format.substr(last_pos);
}

std::string MessageRenderer::ExtractValue(
    std::string_view key, const std::string& format,
    const ws::logging::LogEvent& event) const {
  if (key == kSourceContextKey) {
    return event.SourceContext();
  } else if (key == kMessageKey) {
    return event.Message();
  } else if (key == kLevelKey) {
    return FormatLogLevel(event.Level(), format);
  } else if (key == kTimeStampKey) {
    return FormatTimestamp(event.Timestamp(), format);
  } else if (key == kNewLineKey) {
    return FormatNewLine();
  } else {
    if (auto prop = event.GetProperty(key))
      return Format("{" + format + "}", *prop);
  }

  return "";
}

std::string MessageRenderer::FormatLogLevel(LogLevel level,
                                            const std::string& format) {
  if (format.empty()) return LogLevelToString(level);

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

std::string MessageRenderer::FormatNewLine() { return "\n"; }
}  // namespace logging
}  // namespace ws
