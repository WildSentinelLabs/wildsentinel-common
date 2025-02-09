#include "logging/dispatchers/sync_log_dispatcher.h"

SyncLogDispatcher::SyncLogDispatcher(std::weak_ptr<ILogSink> sink)
    : sink_(sink) {}

void SyncLogDispatcher::Dispatch(const std::string& message) {
  auto sink = sink_.lock();
  if (sink) sink->Display(message);
}

void SyncLogDispatcher::Await() {}
