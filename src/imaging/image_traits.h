#pragma once

#include <type_traits>

#include "imaging/chroma_subsampling.h"
#include "imaging/color_space.h"
#include "imaging/pixel/pixel_allowed_types.h"
namespace ws {
namespace imaging {

class ImageTraits {
 public:
  static uint8_t GetBitAlignment(uint8_t bit_depth);

  static uint8_t GetChromaAlignment(ChromaSubsampling subsampling);

  static bool IsSigned(uint8_t bit_depth);

  template <ws::imaging::IsAllowedPixelNumericType T>
  static constexpr T Clamp(T value, T min_value, T max_value) {
    return value < min_value ? min_value
                             : (value > max_value ? max_value : value);
  }
};
}  // namespace imaging
}  // namespace ws
