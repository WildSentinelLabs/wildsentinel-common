#pragma once

#include <iostream>
#include <memory>
#include <mutex>

#include "logging/dispatchers/async_log_dispatcher.h"
#include "logging/dispatchers/sync_log_dispatcher.h"
#include "logging/ilog_dispatcher.h"
#include "logging/ilog_enricher.h"
#include "logging/ilog_sink.h"
#include "logging/log_level.h"
#include "logging/rendering/message_renderer.h"

class ConsoleLogSink : public ILogSink,
                       public std::enable_shared_from_this<ConsoleLogSink> {
 public:
  explicit ConsoleLogSink(LogLevel min_log_level = LogLevel::kInformation,
                          std::vector<std::shared_ptr<ILogEnricher>> enrichers =
                              std::vector<std::shared_ptr<ILogEnricher>>(),
                          const std::string& template_format =
                              "{Timestamp:%Y-%m-%d %X} [{Level:u3}] "
                              "{Message:lj}{NewLine}{Exception}");

  ~ConsoleLogSink() override;

  void Enable() override;

  void EnableAsync() override;

  void Display(LogEvent event) override;

  void Display(const std::string& message) override;

 private:
  static std::mutex console_mutex_;
  LogLevel min_log_level_;
  std::unique_ptr<ILogDispatcher> dispatcher_;
  std::vector<std::shared_ptr<ILogEnricher>> enrichers_;
  MessageRenderer renderer_;
};
