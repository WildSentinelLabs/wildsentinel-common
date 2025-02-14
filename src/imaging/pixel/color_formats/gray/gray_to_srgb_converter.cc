#include "imaging/pixel/color_formats/gray/gray_to_srgb_converter.h"
namespace ws {
namespace imaging {
namespace pixel {
template <IsAllowedPixelNumericType T>
GrayToSRgbConverter<T>::GrayToSRgbConverter(uint8_t bit_depth)
    : PixelColorConverter<T>(bit_depth){};

template <IsAllowedPixelNumericType T>
void GrayToSRgbConverter<T>::Convert(const Gray<T>& gray, Rgb<T>& rgb) const {
  rgb.r = gray.gray;
  rgb.g = gray.gray;
  rgb.b = gray.gray;
}

template <IsAllowedPixelNumericType T>
void GrayToSRgbConverter<T>::ConvertWithAlpha(const Ya<T>& ya,
                                              Rgba<T>& rgba) const {
  rgba.r = ya.gray;
  rgba.g = ya.gray;
  rgba.b = ya.gray;
  rgba.alpha = ya.alpha;
}
}  // namespace pixel
}  // namespace imaging
}  // namespace ws
