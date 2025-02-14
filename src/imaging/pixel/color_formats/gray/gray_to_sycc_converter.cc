#include "imaging/pixel/color_formats/gray/gray_to_sycc_converter.h"
namespace ws {
namespace imaging {
namespace pixel {
template <IsAllowedPixelNumericType T>
GrayToSYccConverter<T>::GrayToSYccConverter(uint8_t bit_depth)
    : PixelColorConverter<T>(bit_depth) {}

template <IsAllowedPixelNumericType T>
void GrayToSYccConverter<T>::Convert(const Gray<T>& gray, Ycc<T>& ycc) const {
  ycc.y = gray.gray;
  ycc.cb = this->max_value_ / 2;
  ycc.cr = this->max_value_ / 2;
}
}  // namespace pixel
}  // namespace imaging
}  // namespace ws
