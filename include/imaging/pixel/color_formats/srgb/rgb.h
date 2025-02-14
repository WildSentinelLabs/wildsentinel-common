#pragma once

#include "imaging/pixel/pixel_allowed_types.h"

namespace ws {
namespace imaging {
namespace pixel {
template <typename T>
struct Rgb {
  static_assert(IsAllowedPixelComponentType<T>,
                "T must be a valid pixel numeric type");
  T r;  // Red
  T g;  // Green
  T b;  // Blue
};

template <typename T>
struct IsRgbType : std::false_type {};

template <IsAllowedPixelComponentType T>
struct IsRgbType<Rgb<T>> : std::true_type {};
}  // namespace pixel
}  // namespace imaging
}  // namespace ws
