#include "ws/logging/sinks/console_log_sink.h"

namespace ws {
namespace logging {

ConsoleLogSink::ConsoleLogSink(LogLevel min_log_level,
                               const std::string& template_format)
    : min_log_level_(min_log_level),
      renderer_(template_format),
      dispatcher_(nullptr) {
  FormatConsoleOutput();
}

ConsoleLogSink::~ConsoleLogSink() {
  if (dispatcher_) dispatcher_->Await();
}

void ConsoleLogSink::Enable() {
  dispatcher_ = std::make_unique<ws::logging::SyncLogDispatcher>();
}

void ConsoleLogSink::EnableAsync() {
  dispatcher_ = std::make_unique<ws::logging::AsyncLogDispatcher>();
}

void ConsoleLogSink::Display(ws::logging::LogEvent event) {
  if (event.Level() < min_log_level_) return;
  std::string formatted_message = renderer_.Render(event);
  if (dispatcher_) dispatcher_->Dispatch(*this, formatted_message);
}

void ConsoleLogSink::Display(const std::string& message) const {
  Console::WriteLine(message);
}

}  // namespace logging
}  // namespace ws
