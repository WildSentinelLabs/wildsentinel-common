#pragma once

#include <memory>
#include <vector>

#include "logging/ilog_dispatcher.h"
#include "logging/ilog_sink.h"
#include "logging/log_level.h"
namespace ws {
namespace logging {
namespace dispatchers {

class SyncLogDispatcher : public ILogDispatcher {
 public:
  explicit SyncLogDispatcher(std::weak_ptr<ILogSink> sink);

  void Dispatch(const std::string& message) override;

  void Await() override;

 private:
  std::weak_ptr<ILogSink> sink_;
};
}  // namespace dispatchers
}  // namespace logging
}  // namespace ws
