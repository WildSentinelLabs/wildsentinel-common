#pragma once
#include "imaging/pixel/pixel_allowed_types.h"

namespace ws {
namespace imaging {
template <typename T>
struct Cmyka {
  static_assert(IsAllowedPixelComponentType<T>,
                "T must be a valid pixel numeric type");
  T c;            // Cyan
  T m;            // Magenta
  T y;            // Yellow
  T k;            // Black
  T alpha = 255;  // Transparency
};

template <typename T>
struct IsCmykaType : std::false_type {};

template <IsAllowedPixelComponentType T>
struct IsCmykaType<Cmyka<T>> : std::true_type {};
}  // namespace imaging
}  // namespace ws
