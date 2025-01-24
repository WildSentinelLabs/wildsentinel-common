#pragma once

#include "pixel/color_spaces/gray/gray_to_sycc_converter.h"

template <typename T>
GrayToSYCCConverter<T>::GrayToSYCCConverter(uint8_t bit_depth)
    : PixelConverter(bit_depth) {}

template <typename T>
void GrayToSYCCConverter<T>::Convert(const Gray<T>& gray, YCC<T>& ycc) const {
  ycc.y = gray.gray;
  ycc.cb = max_value_ / 2;
  ycc.cr = max_value_ / 2;
}
