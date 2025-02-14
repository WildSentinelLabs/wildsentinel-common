#include "logging/sinks/console_log_sink.h"

#ifdef _WIN32
#include <windows.h>
#endif
namespace ws {
namespace logging {
namespace sinks {

ConsoleLogSink::ConsoleLogSink(LogLevel min_log_level,
                               const std::string& template_format)
    : min_log_level_(min_log_level),
      renderer_(template_format),
      dispatcher_(nullptr) {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
#endif
}

ConsoleLogSink::~ConsoleLogSink() {
  if (dispatcher_) dispatcher_->Await();
}

void ConsoleLogSink::Enable() {
  dispatcher_ = std::make_unique<ws::logging::dispatchers::SyncLogDispatcher>(
      weak_from_this());
}

void ConsoleLogSink::EnableAsync() {
  dispatcher_ = std::make_unique<ws::logging::dispatchers::AsyncLogDispatcher>(
      weak_from_this());
}

void ConsoleLogSink::Display(ws::logging::events::LogEvent event) {
  if (event.GetLevel() < min_log_level_) return;
  std::string formatted_message = renderer_.Render(event);
  if (dispatcher_) dispatcher_->Dispatch(formatted_message);
}

void ConsoleLogSink::Display(const std::string& message) {
  std::lock_guard<std::mutex> lock(console_mutex_);
  std::cout << message << std::endl;
}

std::mutex ConsoleLogSink::console_mutex_;
}  // namespace sinks
}  // namespace logging
}  // namespace ws
