#include "imaging/color_spaces/sycc/sycc_to_srgb_converter.h"

template <typename T>
SYCCToRGBConverter<T>::SYCCToRGBConverter(uint8_t bit_depth)
    : ColorSpaceConverter<T>(bit_depth) {
  coeffs_ = this->GetBT2020Coefficients();
}

template <typename T>
void SYCCToRGBConverter<T>::Convert(const YCC<T>& ycc, RGB<T>& rgb) const {
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

template class SYCCToRGBConverter<uint8_t>;
template class SYCCToRGBConverter<uint16_t>;
template class SYCCToRGBConverter<int16_t>;
template class SYCCToRGBConverter<uint32_t>;
template class SYCCToRGBConverter<int32_t>;
