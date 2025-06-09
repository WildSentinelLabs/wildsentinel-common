#pragma once

#include "imaging/pixel/pixel_allowed_types.h"
namespace ws {
namespace imaging {
namespace pixel {
template <typename T>
struct Ycck {
  static_assert(IsAllowedPixelComponentType<T>,
                "T must be a valid pixel numeric type");
  T y;
  T cb;
  T cr;
  T k;
};

template <typename T>
struct IsYcckType : std::false_type {};

template <IsAllowedPixelComponentType T>
struct IsYcckType<Ycck<T>> : std::true_type {};
}  // namespace pixel
}  // namespace imaging
}  // namespace ws
