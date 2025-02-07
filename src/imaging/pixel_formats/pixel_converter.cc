#include "imaging/color_spaces/color_space_converter.h"

template <typename T>
ColorSpaceConverter<T>::ColorSpaceConverter(uint8_t bit_depth)
    : min_value_(0), max_value_((1 << bit_depth) - 1){};

template class ColorSpaceConverter<uint8_t>;
template class ColorSpaceConverter<uint16_t>;
template class ColorSpaceConverter<int16_t>;
template class ColorSpaceConverter<uint32_t>;
template class ColorSpaceConverter<int32_t>;
