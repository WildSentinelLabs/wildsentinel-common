#include "imaging/color_spaces/sycc/sycc_to_gray_converter.h"

template <typename T>
SYCCToGrayConverter<T>::SYCCToGrayConverter(uint8_t bit_depth)
    : ColorSpaceConverter<T>(bit_depth) {}

template <typename T>
void SYCCToGrayConverter<T>::Convert(const YCC<T>& sycc, Gray<T>& gray) const {
  gray.gray = sycc.y;
}

template class SYCCToGrayConverter<uint8_t>;
template class SYCCToGrayConverter<uint16_t>;
template class SYCCToGrayConverter<int16_t>;
template class SYCCToGrayConverter<uint32_t>;
template class SYCCToGrayConverter<int32_t>;
