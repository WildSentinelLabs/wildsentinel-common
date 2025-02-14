#pragma once

#include "imaging/pixel/color_formats/gray/gray.h"
#include "imaging/pixel/color_formats/srgb/rgb.h"
#include "imaging/pixel/pixel_color_converter.h"
namespace ws {
namespace imaging {
namespace pixel {
template <IsAllowedPixelNumericType T>
class GrayToSRgbConverter : public PixelColorConverter<T> {
 public:
  explicit GrayToSRgbConverter(uint8_t bit_depth);

  void Convert(const Gray<T>& src, Rgb<T>& dst) const;

  void ConvertWithAlpha(const Ya<T>& src, Rgba<T>& dst) const override;
};
}  // namespace pixel
}  // namespace imaging
}  // namespace ws
