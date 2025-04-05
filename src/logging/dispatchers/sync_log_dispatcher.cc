#include "logging/dispatchers/sync_log_dispatcher.h"
namespace ws {
namespace logging {
namespace dispatchers {

SyncLogDispatcher::SyncLogDispatcher() {}

void SyncLogDispatcher::Dispatch(const ILogSink& sink,
                                 const std::string& message) {
  sink.Display(message);
}

void SyncLogDispatcher::Await() {}
}  // namespace dispatchers
}  // namespace logging
}  // namespace ws
