#pragma once

#include <condition_variable>
#include <memory>

#include "ws/concurrency/collections/concurrent_queue.h"
#include "ws/logging/ilog_dispatcher.h"
#include "ws/logging/ilog_sink.h"
#include "ws/logging/log_level.h"
namespace ws {
namespace logging {

class AsyncLogDispatcher : public ILogDispatcher {
 public:
  AsyncLogDispatcher();

  ~AsyncLogDispatcher() override;

  void Dispatch(const ILogSink& sink, const std::string& message) override;
  void Await() override;

 private:
  struct DispatchEvent {
    const ILogSink* sink;
    std::string message;
  };

  void ProcessQueue();

  std::atomic<bool> running_;
  ws::concurrency::ConcurrentQueue<DispatchEvent> log_queue_;
  std::thread log_thread_;
  std::mutex mutex_;
  std::condition_variable cv_;
};

}  // namespace logging
}  // namespace ws
