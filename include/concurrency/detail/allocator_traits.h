#pragma once
#include <type_traits>

#include "concurrency/detail/config.h"

namespace ws {
namespace concurrency {
namespace detail {
template <typename TAllocator>
class AllocatorTraits : public std::allocator_traits<TAllocator> {
  using base_type = std::allocator_traits<TAllocator>;

 public:
  template <typename T>
  using rebind_traits = typename ws::concurrency::detail::AllocatorTraits<
      typename base_type::template allocator_traits<TAllocator>::rebind_alloc<
          T>>;
};

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
  using std::swap;
  swap(lhs, rhs);
}

template <typename TAllocator>
void SwapAllocatorsImpl(TAllocator&, TAllocator&, std::false_type) {}

template <typename TAllocator>
void SwapAllocators(TAllocator& lhs, TAllocator& rhs) {
  using pocs_type =
      typename std::allocator_traits<TAllocator>::propagate_on_container_swap;
  SwapAllocatorsImpl(lhs, rhs, pocs_type());
}

}  // namespace detail
}  // namespace concurrency
}  // namespace ws
