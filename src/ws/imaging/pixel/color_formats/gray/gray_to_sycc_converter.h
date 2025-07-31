#pragma once

#include "ws/imaging/pixel/color_formats/gray/gray.h"
#include "ws/imaging/pixel/color_formats/sycc/ycc.h"
#include "ws/imaging/pixel/pixel_color_converter.h"
namespace ws {
namespace imaging {
template <IsAllowedPixelNumericType T>
class GrayToSYccConverter : public PixelColorConverter<T> {
 public:
  explicit GrayToSYccConverter(uint8_t bit_depth);

  void Convert(const Gray<T>& gray, Ycc<T>& ycc) const;
};
}  // namespace imaging
}  // namespace ws
