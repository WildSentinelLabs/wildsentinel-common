#pragma once
#include <type_traits>

namespace ws {
namespace concurrency {
template <typename Allocator>
class AllocatorTraits : public std::allocator_traits<Allocator> {
  using base_type = std::allocator_traits<Allocator>;

 public:
  template <typename T>
  using rebind_traits = typename ws::concurrency::AllocatorTraits<
      typename base_type::template allocator_traits<Allocator>::rebind_alloc<
          T>>;
};

template <typename Allocator>
void CopyAssignAllocatorsImpl(Allocator& lhs, const Allocator& rhs,
                              std::true_type) {
  lhs = rhs;
}

template <typename Allocator>
void CopyAssignAllocatorsImpl(Allocator&, const Allocator&, std::false_type) {}

template <typename Allocator>
void CopyAssignAllocators(Allocator& lhs, const Allocator& rhs) {
  using pocca_type = typename std::allocator_traits<
      Allocator>::propagate_on_container_copy_assignment;
  CopyAssignAllocatorsImpl(lhs, rhs, pocca_type());
}

template <typename Allocator>
void MoveAssignAllocatorsImpl(Allocator& lhs, Allocator& rhs, std::true_type) {
  lhs = std::move(rhs);
}

template <typename Allocator>
void MoveAssignAllocatorsImpl(Allocator&, Allocator&, std::false_type) {}

template <typename Allocator>
void MoveAssignAllocators(Allocator& lhs, Allocator& rhs) {
  using pocma_type = typename std::allocator_traits<
      Allocator>::propagate_on_container_move_assignment;
  MoveAssignAllocatorsImpl(lhs, rhs, pocma_type());
}

template <typename Allocator>
void SwapAllocatorsImpl(Allocator& lhs, Allocator& rhs, std::true_type) {
  using std::swap;
  swap(lhs, rhs);
}

template <typename Allocator>
void SwapAllocatorsImpl(Allocator&, Allocator&, std::false_type) {}

template <typename Allocator>
void SwapAllocators(Allocator& lhs, Allocator& rhs) {
  using pocs_type =
      typename std::allocator_traits<Allocator>::propagate_on_container_swap;
  SwapAllocatorsImpl(lhs, rhs, pocs_type());
}

}  // namespace concurrency
}  // namespace ws
