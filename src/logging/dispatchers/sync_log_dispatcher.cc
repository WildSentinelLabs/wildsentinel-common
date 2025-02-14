#include "logging/dispatchers/sync_log_dispatcher.h"
namespace ws {
namespace logging {
namespace dispatchers {

SyncLogDispatcher::SyncLogDispatcher(std::weak_ptr<ILogSink> sink)
    : sink_(sink) {}

void SyncLogDispatcher::Dispatch(const std::string& message) {
  auto sink = sink_.lock();
  if (sink) sink->Display(message);
}

void SyncLogDispatcher::Await() {}
}  // namespace dispatchers
}  // namespace logging
}  // namespace ws
