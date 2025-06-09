#pragma once

#include "imaging/pixel/color_formats/gray/gray.h"
#include "imaging/pixel/color_formats/sycc/ycc.h"
#include "imaging/pixel/pixel_color_converter.h"
namespace ws {
namespace imaging {
namespace pixel {
template <IsAllowedPixelNumericType T>
class GrayToSYccConverter : public PixelColorConverter<T> {
 public:
  explicit GrayToSYccConverter(uint8_t bit_depth);

  void Convert(const Gray<T>& gray, Ycc<T>& ycc) const;
};
}  // namespace pixel
}  // namespace imaging
}  // namespace ws
