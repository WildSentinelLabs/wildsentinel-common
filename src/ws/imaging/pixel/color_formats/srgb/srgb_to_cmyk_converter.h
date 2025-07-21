#pragma once

#include <algorithm>

#include "ws/imaging/pixel/color_formats/cmyk/cmyk.h"
#include "ws/imaging/pixel/color_formats/srgb/rgb.h"
#include "ws/imaging/pixel/pixel_color_converter.h"
namespace ws {
namespace imaging {
template <IsAllowedPixelNumericType T>
class SRgbToCmykConverter : public PixelColorConverter<T> {
 public:
  explicit SRgbToCmykConverter(uint8_t bit_depth);

  void Convert(const Rgb<T>& rgb, Cmyk<T>& cmyk) const;
  void ConvertWithAlpha(const Rgba<T>& rgba, Cmyka<T>& cmyka) const;
};
}  // namespace imaging
}  // namespace ws
