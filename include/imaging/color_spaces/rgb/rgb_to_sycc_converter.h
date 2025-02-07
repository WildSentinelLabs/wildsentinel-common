#pragma once

#include "imaging/color_spaces/color_space_converter.h"
#include "imaging/color_spaces/rgb/rgb.h"
#include "imaging/color_spaces/sycc/ycc.h"

template <typename T>
class RGBToSYCCConverter : public ColorSpaceConverter<T> {
 public:
  explicit RGBToSYCCConverter(uint8_t bit_depth);

  void Convert(const RGB<T>& rgb, YCC<T>& ycc) const;

 private:
  RGB<double> coeffs_;
};

// TODO: ADD ENUM FOR BTSUPPORT
