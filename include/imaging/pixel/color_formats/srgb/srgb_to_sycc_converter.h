#pragma once

#include "imaging/pixel/color_formats/srgb/rgb.h"
#include "imaging/pixel/color_formats/sycc/ycc.h"
#include "imaging/pixel/pixel_color_converter.h"
namespace ws {
namespace imaging {
namespace pixel {
template <IsAllowedPixelNumericType T>
class SRgbToSYccConverter : public PixelColorConverter<T> {
 public:
  explicit SRgbToSYccConverter(uint8_t bit_depth);

  void Convert(const Rgb<T>& rgb, Ycc<T>& ycc) const;

 private:
  Rgb<double> coeffs_;
};

// TODO: ADD ENUM FOR BTSUPPORT
}  // namespace pixel
}  // namespace imaging
}  // namespace ws
