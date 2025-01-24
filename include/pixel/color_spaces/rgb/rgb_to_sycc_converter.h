#pragma once

#include "pixel/color_spaces/rgb/rgb.h"
#include "pixel/color_spaces/sycc/ycc.h"
#include "pixel/pixel_converter.h"

template <typename T>
class RGBToSYCCConverter : public PixelConverter {
 public:
  explicit RGBToSYCCConverter(uint8_t bit_depth);

  void Convert(const RGB<T>& rgb, YCC<T>& ycc) const;

 private:
  RGB<double> coeffs_;
};

// TODO: ADD ENUM FOR BTSUPPORT
