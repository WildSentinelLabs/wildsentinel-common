#pragma once

#include <memory>

#include "logging/ilog_dispatcher.h"
#include "logging/ilog_sink.h"
#include "logging/log_level.h"
namespace ws {
namespace logging {

class SyncLogDispatcher : public ILogDispatcher {
 public:
  SyncLogDispatcher();

  void Dispatch(const ILogSink& sink, const std::string& message) override;

  void Await() override;
};

}  // namespace logging
}  // namespace ws
