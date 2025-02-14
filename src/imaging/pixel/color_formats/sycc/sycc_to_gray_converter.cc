#include "imaging/pixel/color_formats/sycc/sycc_to_gray_converter.h"

namespace ws {
namespace imaging {
namespace pixel {
template <IsAllowedPixelNumericType T>
SYccToGrayConverter<T>::SYccToGrayConverter(uint8_t bit_depth)
    : PixelColorConverter<T>(bit_depth) {}

template <IsAllowedPixelNumericType T>
void SYccToGrayConverter<T>::Convert(const Ycc<T>& ycc, Gray<T>& gray) const {
  gray.gray = ycc.y;
}

}  // namespace pixel
}  // namespace imaging
}  // namespace ws
