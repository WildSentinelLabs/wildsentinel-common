#include "imaging/pixel/color_formats/srgb/srgb_to_cmyk_converter.h"

namespace ws {
namespace imaging {
namespace pixel {
template <IsAllowedPixelNumericType T>
SRgbToCmykConverter<T>::SRgbToCmykConverter(uint8_t bit_depth)
    : PixelColorConverter<T>(bit_depth){};

template <IsAllowedPixelNumericType T>
void SRgbToCmykConverter<T>::Convert(const Rgb<T>& rgb, Cmyk<T>& cmyk) const {
  double r = static_cast<double>(rgb.r) / this->max_value_;
  double g = static_cast<double>(rgb.g) / this->max_value_;
  double b = static_cast<double>(rgb.b) / this->max_value_;
  double k = 1.0 - std::max({r, g, b});
  if (k == 1.0) {
    cmyk.c = 0;
    cmyk.m = 0;
    cmyk.y = 0;
  } else {
    cmyk.c = this->max_value_ * ((1.0 - r - k) / (1.0 - k));
    cmyk.m = this->max_value_ * ((1.0 - g - k) / (1.0 - k));
    cmyk.y = this->max_value_ * ((1.0 - b - k) / (1.0 - k));
  }

  cmyk.k = this->max_value_ * k;
}

template <IsAllowedPixelNumericType T>
void SRgbToCmykConverter<T>::ConvertWithAlpha(const Rgba<T>& rgba,
                                              Cmyka<T>& cmyka) const {
  Cmyk<T> cmyk;
  Convert(Rgb<T>{rgba.r, rgba.g, rgba.b}, cmyk);
  cmyka.c = cmyk.c;
  cmyka.m = cmyk.m;
  cmyka.y = cmyk.y;
  cmyka.k = cmyk.k;
  cmyka.alpha = rgba.alpha;
}

template class SRgbToCmykConverter<uint8_t>;
template class SRgbToCmykConverter<int8_t>;
template class SRgbToCmykConverter<uint16_t>;
template class SRgbToCmykConverter<int16_t>;
template class SRgbToCmykConverter<uint32_t>;
template class SRgbToCmykConverter<int32_t>;
}  // namespace pixel
}  // namespace imaging
}  // namespace ws
