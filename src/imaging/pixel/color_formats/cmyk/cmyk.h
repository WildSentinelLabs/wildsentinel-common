#pragma once
#include "imaging/pixel/pixel_allowed_types.h"

namespace ws {
namespace imaging {
template <typename T>
struct Cmyk {
  static_assert(IsAllowedPixelComponentType<T>,
                "T must be a valid pixel numeric type");
  T c;  // Cyan
  T m;  // Magenta
  T y;  // Yellow
  T k;  // Black
};

template <typename T>
struct IsCmykType : std::false_type {};

template <IsAllowedPixelComponentType T>
struct IsCmykType<Cmyk<T>> : std::true_type {};
}  // namespace imaging
}  // namespace ws
