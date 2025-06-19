#include "imaging/pixel/color_formats/sycc/sycc_to_gray_converter.h"

namespace ws {
namespace imaging {

template <IsAllowedPixelNumericType T>
SYccToGrayConverter<T>::SYccToGrayConverter(uint8_t bit_depth)
    : PixelColorConverter<T>(bit_depth) {}

template <IsAllowedPixelNumericType T>
void SYccToGrayConverter<T>::Convert(const Ycc<T>& ycc, Gray<T>& gray) const {
  gray.gray = ycc.y;
}

template class SYccToGrayConverter<uint8_t>;
template class SYccToGrayConverter<int8_t>;
template class SYccToGrayConverter<uint16_t>;
template class SYccToGrayConverter<int16_t>;
template class SYccToGrayConverter<uint32_t>;
template class SYccToGrayConverter<int32_t>;

}  // namespace imaging
}  // namespace ws
