#pragma once
#include <cstdlib>
#include <memory_resource>
#include <utility>

#include "arch/cpu_arch.h"
#include "concurrency/detail/config.h"

namespace ws {
namespace concurrency {
template <typename T>
class WsAllocator {
 public:
  using value_type = T;
  using propagate_on_container_move_assignment = std::true_type;

  using is_always_equal = std::true_type;

  WsAllocator() = default;
  template <typename U>
  WsAllocator(const WsAllocator<U>&) noexcept {}

  _GLIBCXX_NODISCARD T* allocate(std::size_t n) {
    return static_cast<T*>(
        ws::arch::detail::AllocateMemory(n * sizeof(value_type)));
  }

  void deallocate(T* p, std::size_t) { ws::arch::detail::DeallocateMemory(p); }
};

template <typename T, typename TU>
inline bool operator==(const WsAllocator<T>&, const WsAllocator<TU>&) noexcept {
  return true;
}

#if !__CPP20_COMPARISONS_PRESENT
template <typename T, typename TU>
inline bool operator!=(const WsAllocator<T>&, const WsAllocator<TU>&) noexcept {
  return false;
}
#endif

}  // namespace concurrency
}  // namespace ws
