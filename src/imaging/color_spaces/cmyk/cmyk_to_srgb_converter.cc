#include "imaging/color_spaces/cmyk/cmyk_to_srgb_converter.h"

template <typename T>
CMYKToRGBConverter<T>::CMYKToRGBConverter(uint8_t bit_depth)
    : ColorSpaceConverter<T>(bit_depth) {}

template <typename T>
void CMYKToRGBConverter<T>::Convert(const CMYK<T>& cmyk, RGB<T>& rgb) const {
  double c = static_cast<double>(cmyk.c) / this->max_value_;
  double m = static_cast<double>(cmyk.m) / this->max_value_;
  double y = static_cast<double>(cmyk.y) / this->max_value_;
  double k = static_cast<double>(cmyk.k) / this->max_value_;

  rgb.r = this->max_value_ * (1.0 - c) * (1.0 - k);
  rgb.g = this->max_value_ * (1.0 - m) * (1.0 - k);
  rgb.b = this->max_value_ * (1.0 - y) * (1.0 - k);
}

template class CMYKToRGBConverter<uint8_t>;
template class CMYKToRGBConverter<uint16_t>;
template class CMYKToRGBConverter<int16_t>;
template class CMYKToRGBConverter<uint32_t>;
template class CMYKToRGBConverter<int32_t>;

template <typename T>
CMYKAToRGBAConverter<T>::CMYKAToRGBAConverter(uint8_t bit_depth)
    : ColorSpaceConverter<T>(bit_depth) {}

template <typename T>
void CMYKAToRGBAConverter<T>::Convert(const CMYKA<T>& cmyk,
                                      RGBA<T>& rgb) const {
  double c = static_cast<double>(cmyk.c) / this->max_value_;
  double m = static_cast<double>(cmyk.m) / this->max_value_;
  double y = static_cast<double>(cmyk.y) / this->max_value_;
  double k = static_cast<double>(cmyk.k) / this->max_value_;

  rgb.r = this->max_value_ * (1.0 - c) * (1.0 - k);
  rgb.g = this->max_value_ * (1.0 - m) * (1.0 - k);
  rgb.b = this->max_value_ * (1.0 - y) * (1.0 - k);
  rgb.alpha = cmyk.alpha;
}

template class CMYKAToRGBAConverter<uint8_t>;
template class CMYKAToRGBAConverter<uint16_t>;
template class CMYKAToRGBAConverter<int16_t>;
template class CMYKAToRGBAConverter<uint32_t>;
template class CMYKAToRGBAConverter<int32_t>;
