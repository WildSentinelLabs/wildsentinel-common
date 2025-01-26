#pragma once

#include "pixel/color_spaces/gray/gray.h"
#include "pixel/color_spaces/rgb/rgb.h"
#include "pixel/pixel_converter.h"

template <typename T>
class GrayToRGBConverter : public PixelConverter<T> {
 public:
  explicit GrayToRGBConverter(uint8_t bit_depth);

  void Convert(const Gray<T>& gray, RGB<T>& rgb) const;
};

template <typename T>
class YAToRGBAConverter : public PixelConverter<T> {
 public:
  explicit YAToRGBAConverter(uint8_t bit_depth);

  void Convert(const YA<T>& ya, RGBA<T>& rgba) const;
};
