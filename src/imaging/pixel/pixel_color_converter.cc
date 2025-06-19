#include "imaging/pixel/pixel_color_converter.h"
namespace ws {
namespace imaging {

template <IsAllowedPixelNumericType T>
PixelColorConverter<T>::PixelColorConverter(uint8_t bit_depth)
    : min_value_(0), max_value_((1 << bit_depth) - 1){};

template class PixelColorConverter<uint8_t>;
template class PixelColorConverter<int8_t>;
template class PixelColorConverter<uint16_t>;
template class PixelColorConverter<int16_t>;
template class PixelColorConverter<uint32_t>;
template class PixelColorConverter<int32_t>;

}  // namespace imaging
}  // namespace ws
