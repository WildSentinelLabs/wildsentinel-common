#pragma once

#include "imaging/color_spaces/color_space_converter.h"
#include "imaging/color_spaces/gray/gray.h"
#include "imaging/color_spaces/srgb/rgb.h"

template <typename T>
class RGBToGrayConverter : public ColorSpaceConverter<T> {
 public:
  explicit RGBToGrayConverter(uint8_t bit_depth);

  void Convert(const RGB<T>& rgb, Gray<T>& gray) const;

 protected:
  RGB<double> coeffs_;

  static T ConvertWithCoeffs(const RGB<T>& rgb, const RGB<double>& coeffs);

  // Average
  static constexpr RGB<double> GetAverageCoefficients() {
    return {1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0};
  }
};

template <typename T>
class RGBAToYAConverter : public RGBToGrayConverter<T> {
 public:
  explicit RGBAToYAConverter(uint8_t bit_depth);

  void Convert(const RGBA<T>& rgba, YA<T>& ya) const;
};

// TODO: ADD ENUM FOR MethodSUPPORT
