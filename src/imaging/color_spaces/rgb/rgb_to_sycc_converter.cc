#include "imaging/color_spaces/rgb/rgb_to_sycc_converter.h"

template <typename T>
RGBToSYCCConverter<T>::RGBToSYCCConverter(uint8_t bit_depth)
    : ColorSpaceConverter<T>(bit_depth) {
  coeffs_ = this->GetBT2020Coefficients();
}

template <typename T>
void RGBToSYCCConverter<T>::Convert(const RGB<T>& rgb, YCC<T>& ycc) const {
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

template class RGBToSYCCConverter<uint8_t>;
template class RGBToSYCCConverter<uint16_t>;
template class RGBToSYCCConverter<int16_t>;
template class RGBToSYCCConverter<uint32_t>;
template class RGBToSYCCConverter<int32_t>;
