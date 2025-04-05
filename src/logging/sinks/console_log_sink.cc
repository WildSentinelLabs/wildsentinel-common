#include "logging/sinks/console_log_sink.h"

namespace ws {
namespace logging {
namespace sinks {

ConsoleLogSink::ConsoleLogSink(LogLevel min_log_level,
                               const std::string& template_format)
    : min_log_level_(min_log_level),
      renderer_(template_format),
      dispatcher_(nullptr) {
  ws::arch::FormatConsoleOutput();
}

ConsoleLogSink::~ConsoleLogSink() {
  if (dispatcher_) dispatcher_->Await();
}

void ConsoleLogSink::Enable() {
  dispatcher_ = std::make_unique<ws::logging::dispatchers::SyncLogDispatcher>();
}

void ConsoleLogSink::EnableAsync() {
  dispatcher_ =
      std::make_unique<ws::logging::dispatchers::AsyncLogDispatcher>();
}

void ConsoleLogSink::Display(ws::logging::events::LogEvent event) {
  if (event.Level() < min_log_level_) return;
  std::string formatted_message = renderer_.Render(event);
  if (dispatcher_) dispatcher_->Dispatch(*this, formatted_message);
}

void ConsoleLogSink::Display(const std::string& message) const {
  std::lock_guard<std::mutex> lock(console_mutex_);
  std::cout << message << std::endl;
}

std::mutex ConsoleLogSink::console_mutex_;
}  // namespace sinks
}  // namespace logging
}  // namespace ws
