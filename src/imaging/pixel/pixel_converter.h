#pragma once
#include <cstdint>

#include "imaging/pixel/color_formats/cmyk/cmyk.h"
#include "imaging/pixel/color_formats/cmyk/cmyka.h"
#include "imaging/pixel/color_formats/gray/gray.h"
#include "imaging/pixel/color_formats/gray/ya.h"
#include "imaging/pixel/color_formats/srgb/rgb.h"
#include "imaging/pixel/color_formats/srgb/rgba.h"
#include "imaging/pixel/color_formats/sycc/ycc.h"
#include "imaging/pixel/color_formats/sycck/ycck.h"
#include "imaging/pixel/pixel_allowed_types.h"

namespace ws {
namespace imaging {

template <typename T>
concept IsAllowedColorFormat =
    std::bool_constant<IsCmykType<T>::value>::value ||
    std::bool_constant<IsGrayType<T>::value>::value ||
    std::bool_constant<IsRgbType<T>::value>::value ||
    std::bool_constant<IsCmykaType<T>::value>::value ||
    std::bool_constant<IsYaType<T>::value>::value ||
    std::bool_constant<IsRgbaType<T>::value>::value ||
    std::bool_constant<IsYccType<T>::value>::value ||
    std::bool_constant<IsYcckType<T>::value>::value;

class PixelConverter {
 public:
  virtual ~PixelConverter() = default;

  template <IsAllowedColorFormat T1, IsAllowedColorFormat T2>
  void Convert(const T1& src, T2& dst) const;
};

}  // namespace imaging
}  // namespace ws
