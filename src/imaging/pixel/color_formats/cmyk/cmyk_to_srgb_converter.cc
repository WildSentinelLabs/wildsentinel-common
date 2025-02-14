#include "imaging/pixel/color_formats/cmyk/cmyk_to_srgb_converter.h"
namespace ws {
namespace imaging {
namespace pixel {
template <IsAllowedPixelNumericType T>
CmykToSRgbConverter<T>::CmykToSRgbConverter(uint8_t bit_depth)
    : PixelColorConverter<T>(bit_depth) {}

template <IsAllowedPixelNumericType T>
void CmykToSRgbConverter<T>::Convert(const Cmyk<T>& cmyk, Rgb<T>& rgb) const {
  double c = static_cast<double>(cmyk.c) / this->max_value_;
  double m = static_cast<double>(cmyk.m) / this->max_value_;
  double y = static_cast<double>(cmyk.y) / this->max_value_;
  double k = static_cast<double>(cmyk.k) / this->max_value_;

  rgb.r = this->max_value_ * (1.0 - c) * (1.0 - k);
  rgb.g = this->max_value_ * (1.0 - m) * (1.0 - k);
  rgb.b = this->max_value_ * (1.0 - y) * (1.0 - k);
}

template <IsAllowedPixelNumericType T>
void CmykToSRgbConverter<T>::ConvertWithAlpha(const Cmyka<T>& cmyka,
                                              Rgba<T>& rgba) const {
  double c = static_cast<double>(cmyka.c) / this->max_value_;
  double m = static_cast<double>(cmyka.m) / this->max_value_;
  double y = static_cast<double>(cmyka.y) / this->max_value_;
  double k = static_cast<double>(cmyka.k) / this->max_value_;

  rgba.r = this->max_value_ * (1.0 - c) * (1.0 - k);
  rgba.g = this->max_value_ * (1.0 - m) * (1.0 - k);
  rgba.b = this->max_value_ * (1.0 - y) * (1.0 - k);
  rgba.alpha = cmyka.alpha;
}
}  // namespace pixel
}  // namespace imaging
}  // namespace ws
