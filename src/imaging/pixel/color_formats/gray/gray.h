#pragma once
#include "imaging/pixel/pixel_allowed_types.h"

namespace ws {
namespace imaging {
template <typename T>
struct Gray {
  static_assert(IsAllowedPixelComponentType<T>,
                "T must be a valid pixel numeric type");
  T gray;  // Mono
};

template <typename T>
struct IsGrayType : std::false_type {};

template <IsAllowedPixelComponentType T>
struct IsGrayType<Gray<T>> : std::true_type {};
}  // namespace imaging
}  // namespace ws
