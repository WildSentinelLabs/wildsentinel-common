#pragma once

#include "ws/imaging/pixel/color_formats/srgb/rgb.h"
#include "ws/imaging/pixel/color_formats/sycc/ycc.h"
#include "ws/imaging/pixel/pixel_color_converter.h"
namespace ws {
namespace imaging {

template <IsAllowedPixelNumericType T>
class SYccToRgbConverter : public PixelColorConverter<T> {
 public:
  explicit SYccToRgbConverter(
      uint8_t bit_depth,
      PixelColorConverter<T>::DigitalTvStudioEncodingRec rec =
          PixelColorConverter<T>::DigitalTvStudioEncodingRec::kBT2020);

  void Convert(const Ycc<T>& ycc, Rgb<T>& rgb) const;

 private:
  Rgb<double> coeffs_;
};

}  // namespace imaging
}  // namespace ws
