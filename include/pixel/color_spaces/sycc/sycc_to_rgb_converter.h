#pragma once

#include "pixel/color_spaces/rgb/rgb.h"
#include "pixel/color_spaces/sycc/ycc.h"
#include "pixel/pixel_converter.h"

template <typename T>
class SYCCToRGBConverter : public PixelConverter<T> {
 public:
  explicit SYCCToRGBConverter(uint8_t bit_depth);

  void Convert(const YCC<T>& ycc, RGB<T>& rgb) const;

 private:
  RGB<double> coeffs_;
};

// TODO: ADD ENUM FOR BTSUPPORT
