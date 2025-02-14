#pragma once

#include "imaging/pixel/pixel_allowed_types.h"

namespace ws {
namespace imaging {
namespace pixel {
template <typename T>
struct Rgba {
  static_assert(IsAllowedPixelComponentType<T>,
                "T must be a valid pixel numeric type");
  T r;            // Red
  T g;            // Green
  T b;            // Blue
  T alpha = 255;  // Transparency
};

template <typename T>
struct IsRgbaType : std::false_type {};

template <IsAllowedPixelComponentType T>
struct IsRgbaType<Rgba<T>> : std::true_type {};

}  // namespace pixel
}  // namespace imaging
}  // namespace ws
