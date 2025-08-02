// Based on oneTBB (https://github.com/uxlfoundation/oneTBB)
// See THIRD-PARTY-NOTICES

#pragma once

#include "ws/config.h"
#define KEEP_LINUX_ORDER
#include "ws/machine.h"
#undef KEEP_LINUX_ORDER

#include <type_traits>

namespace ws {
namespace concurrency {
namespace internal {

inline void* CacheAlignedAllocate(std::size_t size) {
  const std::size_t kCacheLineSize = ws::internal::CacheLineSize();
  assert(ws::internal::IsPowerOfTwo(kCacheLineSize) && "must be power of two");
  if (size + kCacheLineSize < size) throw std::bad_alloc();
  if (size == 0) size = 1;
  void* result = ws::internal::AlignedAllocate(size, kCacheLineSize);
  if (!result) throw std::bad_alloc();
  assert(ws::internal::IsAligned(result, kCacheLineSize) &&
         "The returned address isn't aligned");
  return result;
}

inline void CacheAlignedDeallocate(void* p) {
  ws::internal::AlignedDeallocate(p);
}

template <typename TAllocator>
void CopyAssignAllocatorsImpl(TAllocator& lhs, const TAllocator& rhs,
                              std::true_type) {
  lhs = rhs;
}

template <typename TAllocator>
void CopyAssignAllocatorsImpl(TAllocator&, const TAllocator&, std::false_type) {
}

template <typename TAllocator>
void CopyAssignAllocators(TAllocator& lhs, const TAllocator& rhs) {
  using pocca_type = typename std::allocator_traits<
      TAllocator>::propagate_on_container_copy_assignment;
  CopyAssignAllocatorsImpl(lhs, rhs, pocca_type());
}

template <typename TAllocator>
void MoveAssignAllocatorsImpl(TAllocator& lhs, TAllocator& rhs,
                              std::true_type) {
  lhs = std::move(rhs);
}

template <typename TAllocator>
void MoveAssignAllocatorsImpl(TAllocator&, TAllocator&, std::false_type) {}

template <typename TAllocator>
void MoveAssignAllocators(TAllocator& lhs, TAllocator& rhs) {
  using pocma_type = typename std::allocator_traits<
      TAllocator>::propagate_on_container_move_assignment;
  MoveAssignAllocatorsImpl(lhs, rhs, pocma_type());
}

template <typename TAllocator>
void SwapAllocatorsImpl(TAllocator& lhs, TAllocator& rhs, std::true_type) {
  std::swap(lhs, rhs);
}

template <typename TAllocator>
void SwapAllocatorsImpl(TAllocator&, TAllocator&, std::false_type) {}

template <typename TAllocator>
void SwapAllocators(TAllocator& lhs, TAllocator& rhs) {
  using pocs_type =
      typename std::allocator_traits<TAllocator>::propagate_on_container_swap;
  SwapAllocatorsImpl(lhs, rhs, pocs_type());
}

}  // namespace internal
}  // namespace concurrency
}  // namespace ws
