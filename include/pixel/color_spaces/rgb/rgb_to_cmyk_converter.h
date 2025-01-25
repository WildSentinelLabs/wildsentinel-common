#pragma once

#include "pixel/color_spaces/cmyk/cmyk.h"
#include "pixel/color_spaces/rgb/rgb.h"
#include "pixel/pixel_converter.h"

template <typename T>
class RGBToCMYKConverter : public PixelConverter<T> {
 public:
  explicit RGBToCMYKConverter(uint8_t bit_depth);

  void Convert(const RGB<T>& rgb, CMYK<T>& cmyk) const;
};

template <typename T>
class RGBAToCMYKAConverter : public PixelConverter<T> {
 public:
  explicit RGBAToCMYKAConverter(uint8_t bit_depth);

  void Convert(const RGBA<T>& rgba, CMYKA<T>& cmyka) const;
};
