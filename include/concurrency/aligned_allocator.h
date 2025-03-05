#pragma once

#include "arch/cpu_arch.h"
#include "concurrency/detail/config.h"

namespace ws {
namespace concurrency {

inline void* CacheAlignedAllocate(std::size_t size) {
  const std::size_t kCacheLineSize = ws::arch::detail::CacheLineSize();
  assert(ws::arch::detail::IsPowerOfTwo(kCacheLineSize) &&
         "must be power of two");
  if (size + kCacheLineSize < size) throw std::bad_alloc();
  if (size == 0) size = 1;
  void* result = ws::arch::detail::AlignedAllocate(size, kCacheLineSize);
  if (!result) throw std::bad_alloc();
  assert(ws::arch::detail::IsAligned(result, kCacheLineSize) &&
         "The returned address isn't aligned");
  return result;
}

inline void CacheAlignedDeallocate(void* p) {
  ws::arch::detail::AlignedDeallocate(p);
}

template <typename T>
class AlignedAllocator {
 public:
  using value_type = T;
  using propagate_on_container_move_assignment = std::true_type;
  using is_always_equal = std::true_type;

  AlignedAllocator() = default;
  template <typename TU>
  AlignedAllocator(const AlignedAllocator<TU>&) noexcept {}

  _GLIBCXX_NODISCARD T* allocate(std::size_t n) {
    return static_cast<T*>(
        ws::concurrency::CacheAlignedAllocate(n * sizeof(value_type)));
  }

  void deallocate(T* p, std::size_t /*n*/) { CacheAlignedDeallocate(p); }

  std::size_t max_size() const noexcept {
    return (~std::size_t(0) - ws::arch::detail::CacheLineSize()) /
           sizeof(value_type);
  }
};

template <typename T, typename TU>
bool operator==(const AlignedAllocator<T>&,
                const AlignedAllocator<TU>&) noexcept {
  return true;
}

#if !__CPP20_COMPARISONS_PRESENT
template <typename T, typename U>
bool operator!=(const AlignedAllocator<T>&,
                const AlignedAllocator<U>&) noexcept {
  return false;
}
#endif

template <typename T, typename TU>
bool operator!=(const AlignedAllocator<T>&,
                const AlignedAllocator<TU>&) noexcept {
  return false;
}
}  // namespace concurrency
}  // namespace ws
