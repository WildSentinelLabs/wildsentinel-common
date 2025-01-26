#include "pixel/pixel_converter.h"

template <typename T>
PixelConverter<T>::PixelConverter(uint8_t bit_depth)
    : min_value_(0), max_value_((1 << bit_depth) - 1){};

template class PixelConverter<uint8_t>;
template class PixelConverter<uint16_t>;
template class PixelConverter<int16_t>;
template class PixelConverter<uint32_t>;
template class PixelConverter<int32_t>;
