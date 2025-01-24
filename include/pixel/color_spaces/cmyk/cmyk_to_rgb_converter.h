#pragma once

#include "pixel/color_spaces/cmyk/cmyk.h"
#include "pixel/color_spaces/rgb/rgb.h"
#include "pixel/pixel_converter.h"

template <typename T>
class CMYKToRGBConverter : public PixelConverter {
 public:
  explicit CMYKToRGBConverter(uint8_t bit_depth);

  void Convert(const CMYK<T>& cmyk, RGB<T>& rgb) const;
};

template <typename T>
class CMYKAToRGBAConverter : public PixelConverter {
 public:
  explicit CMYKAToRGBAConverter(uint8_t bit_depth);

  void Convert(const CMYKA<T>& cmyk, RGBA<T>& rgb) const;
};
