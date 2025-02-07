#pragma once

#include "imaging/color_spaces/color_space_converter.h"
#include "imaging/color_spaces/gray/gray.h"
#include "imaging/color_spaces/srgb/rgb.h"

template <typename T>
class GrayToRGBConverter : public ColorSpaceConverter<T> {
 public:
  explicit GrayToRGBConverter(uint8_t bit_depth);

  void Convert(const Gray<T>& gray, RGB<T>& rgb) const;
};

template <typename T>
class YAToRGBAConverter : public ColorSpaceConverter<T> {
 public:
  explicit YAToRGBAConverter(uint8_t bit_depth);

  void Convert(const YA<T>& ya, RGBA<T>& rgba) const;
};
