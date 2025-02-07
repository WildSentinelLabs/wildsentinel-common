#pragma once
#include <cstdint>

#include "imaging/color_spaces/rgb/rgb.h"

template <typename T>
class ColorSpaceConverter {
 public:
  virtual ~ColorSpaceConverter() = default;

 protected:
  explicit ColorSpaceConverter(uint8_t bit_depth);

  T min_value_;
  T max_value_;

  // BT.601 (SD)
  static constexpr RGB<double> GetBT601Coefficients() {
    return {0.299, 0.587, 0.114};
  }

  // BT.709 (HD)
  static constexpr RGB<double> GetBT709Coefficients() {
    return {0.2126, 0.7152, 0.0722};
  }

  // BT.2020 (UHD)
  static constexpr RGB<double> GetBT2020Coefficients() {
    return {0.2627, 0.6780, 0.0593};
  }

  // BT.2100 (HDR)
  static constexpr RGB<double> GetBT2100Coefficients() {
    return {0.2627, 0.6780, 0.0593};
  }
};
