#pragma once
#include <cstdint>

#include "imaging/pixel/color_formats/cmyk/cmyk.h"
#include "imaging/pixel/color_formats/cmyk/cmyka.h"
#include "imaging/pixel/color_formats/gray/gray.h"
#include "imaging/pixel/color_formats/gray/ya.h"
#include "imaging/pixel/color_formats/srgb/rgb.h"
#include "imaging/pixel/color_formats/srgb/rgba.h"
#include "imaging/pixel/color_formats/sycc/ycc.h"
#include "imaging/pixel/color_formats/sycck/ycck.h"
#include "imaging/pixel/pixel_allowed_types.h"

namespace ws {
namespace imaging {

template <IsAllowedPixelNumericType T>
class PixelColorConverter {
 public:
  virtual ~PixelColorConverter() = default;

 protected:
  // BT.601 (SD)
  static constexpr Rgb<double> BT601Coefficients = {0.299, 0.587, 0.114};
  // BT.709 (HD)
  static constexpr Rgb<double> BT709Coefficients = {0.2126, 0.7152, 0.0722};
  // BT.2020 (UHD)
  static constexpr Rgb<double> BT2020Coefficients = {0.2627, 0.6780, 0.0593};
  // BT.2100 (HDR)
  static constexpr Rgb<double> BT2100Coefficients = {0.2627, 0.6780, 0.0593};

  explicit PixelColorConverter(uint8_t bit_depth);

  T min_value_;
  T max_value_;
};
}  // namespace imaging
}  // namespace ws
