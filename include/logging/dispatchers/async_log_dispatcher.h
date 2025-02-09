#pragma once

#include <memory>
#include <vector>

#include "collections/concurrent/concurrent_queue.h"
#include "logging/ilog_dispatcher.h"
#include "logging/ilog_sink.h"
#include "logging/log_level.h"

class AsyncLogDispatcher : public ILogDispatcher {
 public:
  explicit AsyncLogDispatcher(std::weak_ptr<ILogSink> sink);

  ~AsyncLogDispatcher();

  void Dispatch(const std::string& message) override;

  void Await() override;

 private:
  void ProcessQueue();

  std::atomic<bool> running_;
  std::weak_ptr<ILogSink> sink_;
  ConcurrentQueue<std::string> log_queue_;
  std::thread log_thread_;

  std::mutex mutex_;
  std::condition_variable cv_;
};
