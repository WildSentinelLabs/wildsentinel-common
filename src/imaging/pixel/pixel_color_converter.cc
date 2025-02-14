#include "imaging/pixel/pixel_color_converter.h"
namespace ws {
namespace imaging {
namespace pixel {
template <IsAllowedPixelComponentType T>
PixelColorConverter<T>::PixelColorConverter(uint8_t bit_depth)
    : min_value_(0), max_value_((1 << bit_depth) - 1){};
}  // namespace pixel
}  // namespace imaging
}  // namespace ws
