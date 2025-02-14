#pragma once
#include "imaging/pixel/pixel_allowed_types.h"

namespace ws {
namespace imaging {
namespace pixel {
template <typename T>
struct Ya {
  static_assert(IsAllowedPixelComponentType<T>,
                "T must be a valid pixel numeric type");
  T gray;         // Mono
  T alpha = 255;  // Transparency
};

template <typename T>
struct IsYaType : std::false_type {};

template <IsAllowedPixelComponentType T>
struct IsYaType<Ya<T>> : std::true_type {};
}  // namespace pixel
}  // namespace imaging
}  // namespace ws
