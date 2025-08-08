// Based on oneTBB (https://github.com/uxlfoundation/oneTBB)
// See THIRD-PARTY-NOTICES

#pragma once
#include <cstddef>

#include "ws/concurrency/internal/helpers.h"

namespace ws {
namespace concurrency {
namespace internal {

template <typename T, std::size_t N = 1>
class AlignedSpace {
  alignas(alignof(T)) std::uint8_t aligned_array[N * sizeof(T)];

 public:
  //! Pointer to beginning of array
  T* begin() const {
    return ws::concurrency::internal::templates::punned_cast<T*>(
        &aligned_array);
  }

  //! Pointer to one past last element in array.
  T* end() const { return begin() + N; }
};

}  // namespace internal
}  // namespace concurrency
}  // namespace ws
