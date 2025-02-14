#pragma once

#include "concurrency/detail/cpu_arch.h"

namespace ws {
namespace concurrency {

inline void* CacheAlignedAllocate(std::size_t size) {
  const std::size_t kCacheLineSize = ws::concurrency::detail::CacheLineSize();
  assert(ws::concurrency::detail::IsPowerOfTwo(kCacheLineSize) &&
         "must be power of two");
  if (size + kCacheLineSize < size) throw std::bad_alloc();
  if (size == 0) size = 1;
  void* result = ws::concurrency::detail::AlignedAllocate(size, kCacheLineSize);
  if (!result) throw std::bad_alloc();
  assert(ws::concurrency::detail::IsAligned(result, kCacheLineSize) &&
         "The returned address isn't aligned");
  return result;
}

inline void CacheAlignedDeallocate(void* p) {
  ws::concurrency::detail::AlignedDeallocate(p);
}

template <typename T>
class AlignedAllocator {
 public:
  using value_type = T;
  using propagate_on_container_move_assignment = std::true_type;
  using is_always_equal = std::true_type;

  AlignedAllocator() = default;
  template <typename U>
  AlignedAllocator(const AlignedAllocator<U>&) noexcept {}

  [[nodiscard]] T* allocate(std::size_t n) {
    return static_cast<T*>(
        ws::concurrency::CacheAlignedAllocate(n * sizeof(value_type)));
  }

  void deallocate(T* p, std::size_t /*n*/) { CacheAlignedDeallocate(p); }

  std::size_t max_size() const noexcept {
    return (~std::size_t(0) - ws::concurrency::detail::CacheLineSize()) /
           sizeof(value_type);
  }
};

template <typename T, typename U>
bool operator==(const AlignedAllocator<T>&,
                const AlignedAllocator<U>&) noexcept {
  return true;
}

template <typename T, typename U>
bool operator!=(const AlignedAllocator<T>&,
                const AlignedAllocator<U>&) noexcept {
  return false;
}
}  // namespace concurrency
}  // namespace ws
