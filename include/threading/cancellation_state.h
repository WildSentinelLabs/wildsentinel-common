#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

struct CancellationState {
  std::atomic_bool cancelled{false};
  std::mutex mtx;
  std::unordered_map<size_t, std::function<void()>> callbacks;
  size_t next_id = 1;
};
