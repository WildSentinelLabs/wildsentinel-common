#pragma once

#include "pixel/color_spaces/sycc/sycc_to_rgb_converter.h"

template <typename T>
SYCCToRGBConverter<T>::SYCCToRGBConverter(uint8_t bit_depth)
    : PixelConverter(bit_depth), coeffs_(GetBT2020Coefficients()) {}

template <typename T>
void SYCCToRGBConverter<T>::Convert(const YCC<T>& ycc, RGB<T>& rgb) const {
  double y = static_cast<double>(ycc.y) / max_value_;
  double cb = static_cast<double>(ycc.cb) / max_value_ - 0.5;
  double cr = static_cast<double>(ycc.cr) / max_value_ - 0.5;

  double r = y + (2.0 * (1.0 - coeffs_.r)) * cr;
  double g = y - (2.0 * coeffs_.b * cb + 2.0 * coeffs_.r * cr) / kg_;
  double b = y + (2.0 * (1.0 - coeffs_.b)) * cb;

  rgb.r = static_cast<T>(r * max_value_);
  rgb.g = static_cast<T>(g * max_value_);
  rgb.b = static_cast<T>(b * max_value_);
}

template class SYCCToRGBConverter<uint8_t>;
template class SYCCToRGBConverter<uint16_t>;
template class SYCCToRGBConverter<int16_t>;
template class SYCCToRGBConverter<uint32_t>;
template class SYCCToRGBConverter<int32_t>;
