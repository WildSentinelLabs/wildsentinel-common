#pragma once

#include <type_traits>

#include "imaging/chroma_subsampling.h"
#include "imaging/color_space.h"
#include "imaging/pixel/pixel_allowed_types.h"
namespace ws {
namespace imaging {
struct ImageTraits {
  static constexpr uint8_t GetBitAlignment(uint8_t bit_depth);
  static constexpr uint8_t GetChromaAlignment(ChromaSubsampling subsampling);
  static constexpr bool IsSigned(uint8_t bit_depth);

  template <ws::imaging::IsAllowedPixelNumericType T>
  static constexpr T Clamp(T value, T min_value, T max_value);
};

// ============================================================================
// Implementation details for ImageTraits
// ============================================================================

inline constexpr uint8_t ImageTraits::GetBitAlignment(uint8_t bit_depth) {
  if (bit_depth < 9) {
    return 1;
  } else if (bit_depth < 17) {
    return 2;
  } else if (bit_depth < 33) {
    return 4;
  }

  return 1;
}

inline constexpr uint8_t ImageTraits::GetChromaAlignment(
    ChromaSubsampling subsampling) {
  switch (subsampling) {
    case ChromaSubsampling::kSamp411:
      return 8;
    case ChromaSubsampling::kSamp420:
    case ChromaSubsampling::kSamp422:
      return 4;
    case ChromaSubsampling::kSamp400:
    case ChromaSubsampling::kSamp444:
    case ChromaSubsampling::kSamp440:
    case ChromaSubsampling::kSamp441:
      return 2;
    default:
      return 1;
  }
}

inline constexpr bool ImageTraits::IsSigned(uint8_t bit_depth) {
  return bit_depth >= 9 && bit_depth <= 12;
}

template <ws::imaging::IsAllowedPixelNumericType T>
inline constexpr T ImageTraits::Clamp(T value, T min_value, T max_value) {
  return value < min_value ? min_value
                           : (value > max_value ? max_value : value);
}
}  // namespace imaging
}  // namespace ws
