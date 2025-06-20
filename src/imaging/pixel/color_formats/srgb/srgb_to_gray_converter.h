#pragma once

#include "imaging/pixel/color_formats/gray/gray.h"
#include "imaging/pixel/color_formats/srgb/rgb.h"
#include "imaging/pixel/pixel_color_converter.h"
namespace ws {
namespace imaging {

template <IsAllowedPixelNumericType T>
class SRgbToGrayConverter : public PixelColorConverter<T> {
 public:
  explicit SRgbToGrayConverter(uint8_t bit_depth);

  void Convert(const Rgb<T>& rgb, Gray<T>& gray) const;
  void ConvertWithAlpha(const Rgba<T>& rgba, Ya<T>& ya) const;

 protected:
  Rgb<double> coeffs_;
};

}  // namespace imaging
}  // namespace ws
