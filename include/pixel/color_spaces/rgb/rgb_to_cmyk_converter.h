#pragma once

#include "pixel/color_spaces/cmyk/cmyk.h"
#include "pixel/color_spaces/rgb/rgb.h"
#include "pixel/pixel_converter.h"

template <typename T>
class RGBToCMYKConverter : public PixelConverter {
 public:
  explicit RGBToCMYKConverter(uint8_t bit_depth);

  void Convert(const RGB<T>& rgb, CMYK<T>& cmyk) const;
};
