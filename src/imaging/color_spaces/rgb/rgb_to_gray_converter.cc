#include "imaging/color_spaces/rgb/rgb_to_gray_converter.h"

template <typename T>
RGBToGrayConverter<T>::RGBToGrayConverter(uint8_t bit_depth)
    : ColorSpaceConverter<T>(bit_depth) {
  coeffs_ = this->GetBT2020Coefficients();
}

template <typename T>
void RGBToGrayConverter<T>::Convert(const RGB<T>& rgb, Gray<T>& gray) const {
  gray.gray = ConvertWithCoeffs(rgb, coeffs_);
}

template <typename T>
T RGBToGrayConverter<T>::ConvertWithCoeffs(const RGB<T>& rgb,
                                           const RGB<double>& coeffs) {
  return coeffs.r * rgb.r + coeffs.g * rgb.g + coeffs.b * rgb.b;
}

template class RGBToGrayConverter<uint8_t>;
template class RGBToGrayConverter<uint16_t>;
template class RGBToGrayConverter<int16_t>;
template class RGBToGrayConverter<uint32_t>;
template class RGBToGrayConverter<int32_t>;

template <typename T>
RGBAToYAConverter<T>::RGBAToYAConverter(uint8_t bit_depth)
    : RGBToGrayConverter<T>(bit_depth){};

template <typename T>
void RGBAToYAConverter<T>::Convert(const RGBA<T>& rgba, YA<T>& ya) const {
  Gray<T> gray;
  gray.gray = this->ConvertWithCoeffs({rgba.r, rgba.g, rgba.b}, this->coeffs_);
  ya.gray = gray.gray;
  ya.alpha = rgba.alpha;
}

// TODO: ADD ENUM FOR MethodSUPPORT

template class RGBAToYAConverter<uint8_t>;
template class RGBAToYAConverter<uint16_t>;
template class RGBAToYAConverter<int16_t>;
template class RGBAToYAConverter<uint32_t>;
template class RGBAToYAConverter<int32_t>;
