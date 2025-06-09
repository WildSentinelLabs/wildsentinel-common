#pragma once

#include <condition_variable>
#include <memory>

#include "concurrency/collections/concurrent_queue.h"
#include "logging/ilog_dispatcher.h"
#include "logging/ilog_sink.h"
#include "logging/log_level.h"
namespace ws {
namespace logging {
namespace dispatchers {

class AsyncLogDispatcher : public ILogDispatcher {
 public:
  AsyncLogDispatcher();

  ~AsyncLogDispatcher();

  void Dispatch(const ILogSink& sink, const std::string& message) override;

  void Await() override;

 private:
  struct DispatchEvent {
    const ILogSink* sink;
    std::string message;
  };

  void ProcessQueue();

  std::atomic<bool> running_;
  ws::concurrency::collections::ConcurrentQueue<DispatchEvent> log_queue_;
  std::thread log_thread_;
  std::mutex mutex_;
  std::condition_variable cv_;
};
}  // namespace dispatchers
}  // namespace logging
}  // namespace ws
