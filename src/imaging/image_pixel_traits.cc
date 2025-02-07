#include "imaging/image_pixel_traits.h"

uint8_t ImagePixelTraits::GetBitAlignment(const uint8_t& bit_depth) {
  if (bit_depth == 8) {
    return 1;
  } else if (bit_depth == 16) {
    return 2;
  } else if (bit_depth > 16) {
    return 4;
  }

  return 1;
}

uint8_t ImagePixelTraits::GetChromaAlignment(
    const ChromaSubsampling& subsampling) {
  switch (subsampling) {
    case ChromaSubsampling::kSAMP_411:
      return 8;
    case ChromaSubsampling::kSAMP_420:
    case ChromaSubsampling::kSAMP_422:
      return 4;
    case ChromaSubsampling::kSAMP_400:
    case ChromaSubsampling::kSAMP_444:
    case ChromaSubsampling::kSAMP_440:
    case ChromaSubsampling::kSAMP_441:
      return 2;
    default:
      return 1;
  }
}

bool ImagePixelTraits::IsSigned(const uint8_t& bit_depth) {
  return bit_depth >= 9 && bit_depth <= 12;
}
