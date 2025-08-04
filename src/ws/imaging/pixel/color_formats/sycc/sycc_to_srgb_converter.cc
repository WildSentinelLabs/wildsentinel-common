#include "ws/imaging/pixel/color_formats/sycc/sycc_to_srgb_converter.h"
namespace ws {
namespace imaging {

template <IsAllowedPixelNumericType T>
SYccToRgbConverter<T>::SYccToRgbConverter(
    uint8_t bit_depth, PixelColorConverter<T>::DigitalTvStudioEncodingRec rec)
    : PixelColorConverter<T>(bit_depth),
      coeffs_(PixelColorConverter<T>::GetEncodingCoefficients(rec)) {}

template <IsAllowedPixelNumericType T>
void SYccToRgbConverter<T>::Convert(const Ycc<T>& ycc, Rgb<T>& rgb) const {
  double y = static_cast<double>(ycc.y) / this->max_value_;
  double cb = static_cast<double>(ycc.cb) / this->max_value_ - 0.5;
  double cr = static_cast<double>(ycc.cr) / this->max_value_ - 0.5;

  double r = y + (2.0 * (1.0 - coeffs_.r)) * cr;
  double g = y - (2.0 * coeffs_.b * cb + 2.0 * coeffs_.r * cr) / coeffs_.g;
  double b = y + (2.0 * (1.0 - coeffs_.b)) * cb;

  rgb.r = static_cast<T>(r * this->max_value_);
  rgb.g = static_cast<T>(g * this->max_value_);
  rgb.b = static_cast<T>(b * this->max_value_);
}

template class SYccToRgbConverter<uint8_t>;
template class SYccToRgbConverter<int8_t>;
template class SYccToRgbConverter<uint16_t>;
template class SYccToRgbConverter<int16_t>;
template class SYccToRgbConverter<uint32_t>;
template class SYccToRgbConverter<int32_t>;

}  // namespace imaging
}  // namespace ws
