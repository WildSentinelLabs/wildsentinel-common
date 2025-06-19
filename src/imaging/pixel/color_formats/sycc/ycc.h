#pragma once

#include "imaging/pixel/pixel_allowed_types.h"
namespace ws {
namespace imaging {

template <typename T>
struct Ycc {
  static_assert(IsAllowedPixelComponentType<T>,
                "T must be a valid pixel numeric type");
  T y;
  T cb;
  T cr;
};

template <typename T>
struct IsYccType : std::false_type {};

template <IsAllowedPixelComponentType T>
struct IsYccType<Ycc<T>> : std::true_type {};

}  // namespace imaging
}  // namespace ws
