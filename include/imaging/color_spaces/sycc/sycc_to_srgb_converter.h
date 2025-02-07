#pragma once

#include "imaging/color_spaces/color_space_converter.h"
#include "imaging/color_spaces/srgb/rgb.h"
#include "imaging/color_spaces/sycc/ycc.h"

template <typename T>
class SYCCToRGBConverter : public ColorSpaceConverter<T> {
 public:
  explicit SYCCToRGBConverter(uint8_t bit_depth);

  void Convert(const YCC<T>& ycc, RGB<T>& rgb) const;

 private:
  RGB<double> coeffs_;
};

// TODO: ADD ENUM FOR BTSUPPORT
