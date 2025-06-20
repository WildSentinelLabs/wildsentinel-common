#include "imaging/pixel/color_formats/srgb/srgb_to_sycc_converter.h"

namespace ws {
namespace imaging {
template <IsAllowedPixelNumericType T>
SRgbToSYccConverter<T>::SRgbToSYccConverter(uint8_t bit_depth)
    : PixelColorConverter<T>(bit_depth),
      coeffs_(PixelColorConverter<T>::BT2020Coefficients()) {}

template <IsAllowedPixelNumericType T>
void SRgbToSYccConverter<T>::Convert(const Rgb<T>& rgb, Ycc<T>& ycc) const {
  double r = static_cast<double>(rgb.r) / this->max_value_;
  double g = static_cast<double>(rgb.g) / this->max_value_;
  double b = static_cast<double>(rgb.b) / this->max_value_;

  double y = coeffs_.r * r + coeffs_.g * g + coeffs_.b * b;
  double cb = 0.5 * ((b - y) / (1.0 - coeffs_.b)) + 0.5;
  double cr = 0.5 * ((r - y) / (1.0 - coeffs_.r)) + 0.5;

  ycc.y = this->max_value_ * y;
  ycc.cb = this->max_value_ * cb;
  ycc.cr = this->max_value_ * cr;
}
// TODO: ADD ENUM FOR MethodSUPPORT

template class SRgbToSYccConverter<uint8_t>;
template class SRgbToSYccConverter<int8_t>;
template class SRgbToSYccConverter<uint16_t>;
template class SRgbToSYccConverter<int16_t>;
template class SRgbToSYccConverter<uint32_t>;
template class SRgbToSYccConverter<int32_t>;
}  // namespace imaging
}  // namespace ws
