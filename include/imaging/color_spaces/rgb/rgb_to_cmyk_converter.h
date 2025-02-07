#pragma once

#include <algorithm>

#include "imaging/color_spaces/cmyk/cmyk.h"
#include "imaging/color_spaces/color_space_converter.h"
#include "imaging/color_spaces/rgb/rgb.h"

template <typename T>
class RGBToCMYKConverter : public ColorSpaceConverter<T> {
 public:
  explicit RGBToCMYKConverter(uint8_t bit_depth);

  void Convert(const RGB<T>& rgb, CMYK<T>& cmyk) const;
};

template <typename T>
class RGBAToCMYKAConverter : public ColorSpaceConverter<T> {
 public:
  explicit RGBAToCMYKAConverter(uint8_t bit_depth);

  void Convert(const RGBA<T>& rgba, CMYKA<T>& cmyka) const;
};
