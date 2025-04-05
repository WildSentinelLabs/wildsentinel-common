#include "logging/dispatchers/async_log_dispatcher.h"
namespace ws {
namespace logging {
namespace dispatchers {
AsyncLogDispatcher::AsyncLogDispatcher()
    : running_(true), log_thread_(&AsyncLogDispatcher::ProcessQueue, this) {}

AsyncLogDispatcher::~AsyncLogDispatcher() {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
  }
  cv_.notify_one();
  if (log_thread_.joinable()) {
    log_thread_.join();
  }
}

void AsyncLogDispatcher::Dispatch(const ILogSink& sink,
                                  const std::string& message) {
  log_queue_.Push({&sink, message});
  cv_.notify_one();
}

void AsyncLogDispatcher::ProcessQueue() {
  while (true) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      cv_.wait(lock, [this] { return !running_ || !log_queue_.Empty(); });

      if (!running_ && log_queue_.Empty()) {
        return;
      }
    }

    DispatchEvent event;
    while (log_queue_.TryPop(event)) {
      event.sink->Display(event.message);
    }
  }
}

void AsyncLogDispatcher::Await() {
  std::unique_lock<std::mutex> lock(mutex_);
  cv_.wait(lock, [this] { return log_queue_.Empty(); });
}
}  // namespace dispatchers
}  // namespace logging
}  // namespace ws
