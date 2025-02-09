#include "logging/sinks/console_log_sink.h"

#ifdef _WIN32
#include <windows.h>
#endif

ConsoleLogSink::ConsoleLogSink(
    LogLevel min_log_level,
    std::vector<std::shared_ptr<ILogEnricher>> enrichers,
    const std::string& template_format)
    : enrichers_(enrichers),
      min_log_level_(min_log_level),
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
  dispatcher_ = std::make_unique<SyncLogDispatcher>(weak_from_this());
}

void ConsoleLogSink::EnableAsync() {
  dispatcher_ = std::make_unique<AsyncLogDispatcher>(weak_from_this());
}

void ConsoleLogSink::Display(LogEvent event) {
  if (event.GetLevel() < min_log_level_) return;
  std::string formatted_message = renderer_.Render(event);
  if (dispatcher_) dispatcher_->Dispatch(formatted_message);
}

void ConsoleLogSink::Display(const std::string& message) {
  std::lock_guard<std::mutex> lock(console_mutex_);
  std::cout << message << std::endl;
}

std::mutex ConsoleLogSink::console_mutex_;
