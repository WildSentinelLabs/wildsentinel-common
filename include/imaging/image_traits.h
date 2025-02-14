#pragma once

#include <type_traits>

#include "imaging/chroma_subsampling.h"
#include "imaging/color_space.h"
#include "imaging/pixel/pixel_allowed_types.h"
namespace ws {
namespace imaging {

class ImageTraits {
 public:
  static uint8_t GetBitAlignment(const uint8_t& bit_depth);

  static uint8_t GetChromaAlignment(const ChromaSubsampling& subsampling);

  static bool IsSigned(const uint8_t& bit_depth);

  template <ws::imaging::pixel::IsAllowedPixelNumericType T>
  static constexpr T Clamp(T value, T min_value, T max_value) {
    return value < min_value ? min_value
                             : (value > max_value ? max_value : value);
  }
};
}  // namespace imaging
}  // namespace ws
