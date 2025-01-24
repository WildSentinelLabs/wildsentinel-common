#pragma once

#include "pixel/color_spaces/sycc/sycc_to_gray_converter.h"

template <typename T>
SYCCToGrayConverter<T>::SYCCToGrayConverter(uint8_t bit_depth)
    : PixelConverter(bit_depth) {}

template <typename T>
void SYCCToGrayConverter<T>::Convert(const YCC<T>& sycc, Gray<T>& gray) const {
  gray.gray = sycc.y;
}
