#pragma once

#include <type_traits>

#include "imaging/chroma_subsampling.h"
#include "imaging/color_space.h"

class ImageTraits {
 public:
  static uint8_t GetBitAlignment(const uint8_t& bit_depth);

  static uint8_t GetChromaAlignment(const ChromaSubsampling& subsampling);

  static bool IsSigned(const uint8_t& bit_depth);

  template <typename T, typename = typename std::enable_if<
                            std::is_arithmetic<T>::value>::type>
  static constexpr T Clamp(T value, T min_value, T max_value) {
    return value < min_value ? min_value
                             : (value > max_value ? max_value : value);
  }
};
