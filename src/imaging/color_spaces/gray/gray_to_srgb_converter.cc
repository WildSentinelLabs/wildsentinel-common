#include "imaging/color_spaces/gray/gray_to_srgb_converter.h"

template <typename T>
GrayToRGBConverter<T>::GrayToRGBConverter(uint8_t bit_depth)
    : ColorSpaceConverter<T>(bit_depth){};

template <typename T>
void GrayToRGBConverter<T>::Convert(const Gray<T>& gray, RGB<T>& rgb) const {
  rgb.r = gray.gray;
  rgb.g = gray.gray;
  rgb.b = gray.gray;
}

template class GrayToRGBConverter<uint8_t>;
template class GrayToRGBConverter<uint16_t>;
template class GrayToRGBConverter<int16_t>;
template class GrayToRGBConverter<uint32_t>;
template class GrayToRGBConverter<int32_t>;

template <typename T>
YAToRGBAConverter<T>::YAToRGBAConverter(uint8_t bit_depth)
    : ColorSpaceConverter<T>(bit_depth){};

template <typename T>
void YAToRGBAConverter<T>::Convert(const YA<T>& ya, RGBA<T>& rgba) const {
  rgba.r = ya.gray;
  rgba.g = ya.gray;
  rgba.b = ya.gray;
  rgba.alpha = ya.alpha;
}

template class YAToRGBAConverter<uint8_t>;
template class YAToRGBAConverter<uint16_t>;
template class YAToRGBAConverter<int16_t>;
template class YAToRGBAConverter<uint32_t>;
template class YAToRGBAConverter<int32_t>;
