#pragma once

#include "pixel/color_spaces/rgb/rgb_to_cmyk_converter.h"

template <typename T>
RGBToCMYKConverter<T>::RGBToCMYKConverter(uint8_t bit_depth)
    : PixelConverter(bit_depth){};

template <typename T>
void RGBToCMYKConverter<T>::Convert(const RGB<T>& rgb, CMYK<T>& cmyk) const {
  double r = static_cast<double>(rgb.r) / max_value_;
  double g = static_cast<double>(rgb.g) / max_value_;
  double b = static_cast<double>(rgb.b) / max_value_;
  double k = 1.0 - std::max({r, g, b});
  switch (k) {
    case 1.0:
      cmyk.c = 0;
      cmyk.m = 0;
      cmyk.y = 0;
      break;

    default:
      cmyk.c = max_value_ * ((1.0 - r - k) / (1.0 - k));
      cmyk.m = max_value_ * ((1.0 - g - k) / (1.0 - k));
      cmyk.y = max_value_ * ((1.0 - b - k) / (1.0 - k));
      break;
  }

  cmyk.k = max_value_ * k;
}
