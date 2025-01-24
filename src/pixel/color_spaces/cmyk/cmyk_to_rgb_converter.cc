#pragma once

#include "pixel/color_spaces/cmyk/cmyk_to_rgb_converter.h"

template <typename T>
CMYKToRGBConverter<T>::CMYKToRGBConverter(uint8_t bit_depth)
    : PixelConverter(bit_depth) {}

template <typename T>
void CMYKToRGBConverter<T>::Convert(const CMYK<T>& cmyk, RGB<T>& rgb) const {
  double c = static_cast<double>(cmyk.c) / max_value_;
  double m = static_cast<double>(cmyk.m) / max_value_;
  double y = static_cast<double>(cmyk.y) / max_value_;
  double k = static_cast<double>(cmyk.k) / max_value_;

  rgb.r = max_value_ * (1.0 - c) * (1.0 - k);
  rgb.g = max_value_ * (1.0 - m) * (1.0 - k);
  rgb.b = max_value_ * (1.0 - y) * (1.0 - k);
}

template <typename T>
CMYKAToRGBAConverter<T>::CMYKAToRGBAConverter(uint8_t bit_depth)
    : PixelConverter(bit_depth) {}

template <typename T>
void CMYKAToRGBAConverter<T>::Convert(const CMYKA<T>& cmyk,
                                      RGBA<T>& rgb) const {
  double c = static_cast<double>(cmyk.c) / max_value_;
  double m = static_cast<double>(cmyk.m) / max_value_;
  double y = static_cast<double>(cmyk.y) / max_value_;
  double k = static_cast<double>(cmyk.k) / max_value_;

  rgb.r = max_value_ * (1.0 - c) * (1.0 - k);
  rgb.g = max_value_ * (1.0 - m) * (1.0 - k);
  rgb.b = max_value_ * (1.0 - y) * (1.0 - k);
  rgb.alpha = cmyk.alpha;
}
