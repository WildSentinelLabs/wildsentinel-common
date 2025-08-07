#pragma once

#include "ws/logging/dispatchers/async_log_dispatcher.h"
#include "ws/logging/dispatchers/sync_log_dispatcher.h"
#include "ws/logging/ilog_dispatcher.h"
#include "ws/logging/ilog_enricher.h"
#include "ws/logging/ilog_sink.h"
#include "ws/logging/log_level.h"
#include "ws/logging/rendering/message_renderer.h"
#include "ws/system.h"
namespace ws {
namespace logging {
class ConsoleLogSink : public ILogSink {
 public:
  ConsoleLogSink(LogLevel min_log_level = LogLevel::kInformation,
                 const std::string& template_format =
                     "{Timestamp:%Y-%m-%d %X} [{Level:u3}] "
                     "{Message:lj}{NewLine}{Exception}");

  ~ConsoleLogSink() override;

  void Enable() override;
  void EnableAsync() override;
  void Display(ws::logging::LogEvent event) override;
  void Display(const std::string& message) const override;

 private:
  LogLevel min_log_level_;
  std::unique_ptr<ILogDispatcher> dispatcher_;
  ws::logging::MessageRenderer renderer_;
};

}  // namespace logging
}  // namespace ws
