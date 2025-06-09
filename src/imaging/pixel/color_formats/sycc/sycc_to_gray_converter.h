#pragma once

#include "imaging/pixel/color_formats/gray/gray.h"
#include "imaging/pixel/color_formats/sycc/ycc.h"
#include "imaging/pixel/pixel_color_converter.h"
namespace ws {
namespace imaging {
namespace pixel {
template <IsAllowedPixelNumericType T>
class SYccToGrayConverter : public PixelColorConverter<T> {
 public:
  explicit SYccToGrayConverter(uint8_t bit_depth);

  void Convert(const Ycc<T>& ycc, Gray<T>& gray) const;
};
}  // namespace pixel
}  // namespace imaging
}  // namespace ws
