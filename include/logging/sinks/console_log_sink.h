#pragma once

#include <memory>
#include <mutex>

#include "arch/cpu_arch.h"
#include "logging/dispatchers/async_log_dispatcher.h"
#include "logging/dispatchers/sync_log_dispatcher.h"
#include "logging/ilog_dispatcher.h"
#include "logging/ilog_enricher.h"
#include "logging/ilog_sink.h"
#include "logging/log_level.h"
#include "logging/rendering/message_renderer.h"
namespace ws {
namespace logging {
namespace sinks {

class ConsoleLogSink : public ILogSink {
 public:
  ConsoleLogSink(LogLevel min_log_level = LogLevel::kInformation,
                 const std::string& template_format =
                     "{Timestamp:%Y-%m-%d %X} [{Level:u3}] "
                     "{Message:lj}{NewLine}{Exception}");

  ~ConsoleLogSink() override;

  void Enable() override;

  void EnableAsync() override;

  void Display(ws::logging::events::LogEvent event) override;

  void Display(const std::string& message) const override;

 private:
  LogLevel min_log_level_;
  std::unique_ptr<ILogDispatcher> dispatcher_;
  ws::logging::rendering::MessageRenderer renderer_;
};
}  // namespace sinks
}  // namespace logging
}  // namespace ws
