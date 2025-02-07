#pragma once

#include "imaging/color_spaces/cmyk/cmyk.h"
#include "imaging/color_spaces/color_space_converter.h"
#include "imaging/color_spaces/rgb/rgb.h"

template <typename T>
class CMYKToRGBConverter : public ColorSpaceConverter<T> {
 public:
  explicit CMYKToRGBConverter(uint8_t bit_depth);

  void Convert(const CMYK<T>& cmyk, RGB<T>& rgb) const;
};

template <typename T>
class CMYKAToRGBAConverter : public ColorSpaceConverter<T> {
 public:
  explicit CMYKAToRGBAConverter(uint8_t bit_depth);

  void Convert(const CMYKA<T>& cmyk, RGBA<T>& rgb) const;
};
