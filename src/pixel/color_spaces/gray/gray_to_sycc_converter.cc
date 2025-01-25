#include "pixel/color_spaces/gray/gray_to_sycc_converter.h"

template <typename T>
GrayToSYCCConverter<T>::GrayToSYCCConverter(uint8_t bit_depth)
    : PixelConverter<T>(bit_depth) {}

template <typename T>
void GrayToSYCCConverter<T>::Convert(const Gray<T>& gray, YCC<T>& ycc) const {
  ycc.y = gray.gray;
  ycc.cb = this->max_value_ / 2;
  ycc.cr = this->max_value_ / 2;
}

template class GrayToSYCCConverter<uint8_t>;
template class GrayToSYCCConverter<uint16_t>;
template class GrayToSYCCConverter<int16_t>;
template class GrayToSYCCConverter<uint32_t>;
template class GrayToSYCCConverter<int32_t>;
