#pragma once

#include "pixel/color_spaces/rgb/rgb_to_cmyk_converter.h"

template <typename T>
RGBToCMYKConverter<T>::RGBToCMYKConverter(uint8_t bit_depth)
    : PixelConverter(bit_depth){};

template <typename T>
void RGBToCMYKConverter<T>::Convert(const RGB<T>& rgb, CMYK<T>& cmyk) const {
  double r = static_cast<double>(rgb.r) / max_value_;
  double g = static_cast<double>(rgb.g) / max_value_;
  double b = static_cast<double>(rgb.b) / max_value_;
  double k = 1.0 - std::max({r, g, b});
  switch (k) {
    case 1.0:
      cmyk.c = 0;
      cmyk.m = 0;
      cmyk.y = 0;
      break;

    default:
      cmyk.c = max_value_ * ((1.0 - r - k) / (1.0 - k));
      cmyk.m = max_value_ * ((1.0 - g - k) / (1.0 - k));
      cmyk.y = max_value_ * ((1.0 - b - k) / (1.0 - k));
      break;
  }

  cmyk.k = max_value_ * k;
}

template class RGBToCMYKConverter<uint8_t>;
template class RGBToCMYKConverter<uint16_t>;
template class RGBToCMYKConverter<int16_t>;
template class RGBToCMYKConverter<uint32_t>;
template class RGBToCMYKConverter<int32_t>;

template <typename T>
RGBAToCMYKAConverter<T>::RGBAToCMYKAConverter(uint8_t bit_depth)
    : PixelConverter(bit_depth){};

template <typename T>
void RGBAToCMYKAConverter<T>::Convert(const RGBA<T>& rgba,
                                      CMYKA<T>& cmyka) const {
  double r = static_cast<double>(rgb.r) / max_value_;
  double g = static_cast<double>(rgb.g) / max_value_;
  double b = static_cast<double>(rgb.b) / max_value_;
  double k = 1.0 - std::max({r, g, b});
  switch (k) {
    case 1.0:
      cmyka.c = 0;
      cmyka.m = 0;
      cmyka.y = 0;
      break;

    default:
      cmyka.c = max_value_ * ((1.0 - r - k) / (1.0 - k));
      cmyka.m = max_value_ * ((1.0 - g - k) / (1.0 - k));
      cmyka.y = max_value_ * ((1.0 - b - k) / (1.0 - k));
      break;
  }

  cmyka.k = max_value_ * k;
  cmyka.alpha = rgba.alpha;
}

template class RGBAToCMYKAConverter<uint8_t>;
template class RGBAToCMYKAConverter<uint16_t>;
template class RGBAToCMYKAConverter<int16_t>;
template class RGBAToCMYKAConverter<uint32_t>;
template class RGBAToCMYKAConverter<int32_t>;
