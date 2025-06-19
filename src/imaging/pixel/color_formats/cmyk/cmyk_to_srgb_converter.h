#pragma once

#include "imaging/pixel/color_formats/cmyk/cmyk.h"
#include "imaging/pixel/color_formats/cmyk/cmyka.h"
#include "imaging/pixel/color_formats/srgb/rgb.h"
#include "imaging/pixel/pixel_color_converter.h"
namespace ws {
namespace imaging {
template <IsAllowedPixelNumericType T>
class CmykToSRgbConverter : public PixelColorConverter<T> {
 public:
  explicit CmykToSRgbConverter(uint8_t bit_depth);

  void Convert(const Cmyk<T>& cmyk, Rgb<T>& rgb) const;

  void ConvertWithAlpha(const Cmyka<T>& cmyka, Rgba<T>& rgba) const;
};
}  // namespace imaging
}  // namespace ws
