#include "imaging/pixel/color_formats/srgb/srgb_to_gray_converter.h"

namespace ws {
namespace imaging {
namespace pixel {
template <IsAllowedPixelNumericType T>
SRgbToGrayConverter<T>::SRgbToGrayConverter(uint8_t bit_depth)
    : PixelColorConverter<T>(bit_depth),
      coeffs_(PixelColorConverter<T>::GetBT2020Coefficients()) {}

template <IsAllowedPixelNumericType T>
void SRgbToGrayConverter<T>::Convert(const Rgb<T>& rgb, Gray<T>& gray) const {
  gray.gray = coeffs_.r * rgb.r + coeffs_.g * rgb.g + coeffs_.b * rgb.b;
}

template <IsAllowedPixelNumericType T>
void SRgbToGrayConverter<T>::ConvertWithAlpha(const Rgba<T>& rgba,
                                              Ya<T>& ya) const {
  Gray<T> gray;
  Convert(Rgb<T>{rgba.r, rgba.g, rgba.b}, gray);
  ya.gray = gray.gray;
  ya.alpha = rgba.alpha;
}
// TODO: ADD ENUM FOR MethodSUPPORT

template class SRgbToGrayConverter<uint8_t>;
template class SRgbToGrayConverter<int8_t>;
template class SRgbToGrayConverter<uint16_t>;
template class SRgbToGrayConverter<int16_t>;
template class SRgbToGrayConverter<uint32_t>;
template class SRgbToGrayConverter<int32_t>;
}  // namespace pixel
}  // namespace imaging
}  // namespace ws
