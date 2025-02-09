#include "logging/dispatchers/async_log_dispatcher.h"

AsyncLogDispatcher::AsyncLogDispatcher(std::weak_ptr<ILogSink> sink)
    : sink_(sink),
      running_(true),
      log_thread_(&AsyncLogDispatcher::ProcessQueue, this) {}

AsyncLogDispatcher::~AsyncLogDispatcher() {
  Await();
  running_ = false;
  cv_.notify_all();
  log_thread_.join();
}

void AsyncLogDispatcher::Dispatch(const std::string& message) {
  {
    std::lock_guard<std::mutex> lock(mutex_);
  }

  log_queue_.Push(message);
}

void AsyncLogDispatcher::ProcessQueue() {
  while (running_ || !log_queue_.Empty()) {
    std::string message;
    log_queue_.WaitAndPop(message);
    auto sink = sink_.lock();
    if (sink) sink->Display(message);

    {
      std::lock_guard<std::mutex> lock(mutex_);
    }
    cv_.notify_all();
  }

  cv_.notify_all();
}

void AsyncLogDispatcher::Await() {
  std::unique_lock<std::mutex> lock(mutex_);
  cv_.wait(lock, [this] { return !running_ && log_queue_.Empty(); });
}
