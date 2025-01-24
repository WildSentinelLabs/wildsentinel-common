#pragma once

#include "pixel/color_spaces/rgb/rgb_to_gray_converter.h"

template <typename T>
RGBToGrayConverter<T>::RGBToGrayConverter(uint8_t bit_depth)
    : PixelConverter(bit_depth), coeffs_(GetBT2020Coefficients()){};

template <typename T>
void RGBToGrayConverter<T>::Convert(const RGB<T>& rgb, Gray<T>& gray) const {
  gray.gray = ConvertWithCoeffs(rgb, coefs_);
}

template <typename T>
T RGBToGrayConverter<T>::ConvertWithCoeffs(const RGB<T>& rgb,
                                           const RGB<double>& coeffs) {
  return coeffs.r * rgb.r + coeffs.g * rgb.g + coeffs.b * rgb.b;
}

template <typename T>
RGBAToYAConverter<T>::RGBAToYAConverter(uint8_t bit_depth)
    : RGBToGrayConverter(bit_depth){};

template <typename T>
void RGBAToYAConverter<T>::Convert(const RGBA<T>& rgba, YA<T>& ya) const {
  Gray<T> gray;
  gray.gray = ConvertWithCoeffs({rgba.r, rgba.g, rgba.b}, coeffs_);
  ya.gray = gray.gray;
  ya.alpha = rgba.alpha;
}

// TODO: ADD ENUM FOR MethodSUPPORT
