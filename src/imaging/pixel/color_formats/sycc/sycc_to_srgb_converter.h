#pragma once

#include "imaging/pixel/color_formats/srgb/rgb.h"
#include "imaging/pixel/color_formats/sycc/ycc.h"
#include "imaging/pixel/pixel_color_converter.h"
namespace ws {
namespace imaging {
namespace pixel {
template <IsAllowedPixelNumericType T>
class SYccToRgbConverter : public PixelColorConverter<T> {
 public:
  explicit SYccToRgbConverter(uint8_t bit_depth);

  void Convert(const Ycc<T>& ycc, Rgb<T>& rgb) const;

 private:
  Rgb<double> coeffs_;
};
}  // namespace pixel
}  // namespace imaging
}  // namespace ws
