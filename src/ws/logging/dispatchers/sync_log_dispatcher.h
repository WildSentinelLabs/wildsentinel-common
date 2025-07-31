#pragma once

#include <memory>

#include "ws/logging/ilog_dispatcher.h"
#include "ws/logging/ilog_sink.h"
#include "ws/logging/log_level.h"
namespace ws {
namespace logging {

class SyncLogDispatcher : public ILogDispatcher {
 public:
  SyncLogDispatcher() = default;

  ~SyncLogDispatcher() override = default;

  void Dispatch(const ILogSink& sink, const std::string& message) override;
  void Await() override;
};

// ============================================================================
// Implementation details for SyncLogDispatcher
// ============================================================================

inline void SyncLogDispatcher::Dispatch(const ILogSink& sink,
                                        const std::string& message) {
  sink.Display(message);
}

inline void SyncLogDispatcher::Await() {}

}  // namespace logging
}  // namespace ws
