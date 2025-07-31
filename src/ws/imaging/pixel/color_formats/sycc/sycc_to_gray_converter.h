#pragma once

#include "ws/imaging/pixel/color_formats/gray/gray.h"
#include "ws/imaging/pixel/color_formats/sycc/ycc.h"
#include "ws/imaging/pixel/pixel_color_converter.h"
namespace ws {
namespace imaging {

template <IsAllowedPixelNumericType T>
class SYccToGrayConverter : public PixelColorConverter<T> {
 public:
  explicit SYccToGrayConverter(uint8_t bit_depth);

  void Convert(const Ycc<T>& ycc, Gray<T>& gray) const;
};

}  // namespace imaging
}  // namespace ws
