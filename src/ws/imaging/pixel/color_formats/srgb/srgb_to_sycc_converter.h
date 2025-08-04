#pragma once

#include "ws/imaging/pixel/color_formats/srgb/rgb.h"
#include "ws/imaging/pixel/color_formats/sycc/ycc.h"
#include "ws/imaging/pixel/pixel_color_converter.h"
namespace ws {
namespace imaging {

template <IsAllowedPixelNumericType T>
class SRgbToSYccConverter : public PixelColorConverter<T> {
 public:
  explicit SRgbToSYccConverter(
      uint8_t bit_depth,
      PixelColorConverter<T>::DigitalTvStudioEncodingRec rec =
          PixelColorConverter<T>::DigitalTvStudioEncodingRec::kBT2020);

  void Convert(const Rgb<T>& rgb, Ycc<T>& ycc) const;

 private:
  Rgb<double> coeffs_;
};

}  // namespace imaging
}  // namespace ws
