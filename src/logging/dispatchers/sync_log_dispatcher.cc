#include "logging/dispatchers/sync_log_dispatcher.h"
namespace ws {
namespace logging {

SyncLogDispatcher::SyncLogDispatcher() {}

void SyncLogDispatcher::Dispatch(const ILogSink& sink,
                                 const std::string& message) {
  sink.Display(message);
}

void SyncLogDispatcher::Await() {}

}  // namespace logging
}  // namespace ws
