#include "imaging/pixel_align.h"

uint8_t PixelAlign::GetBitAlignment(const uint8_t& bit_depth) {
  if (bit_depth == 8) {
    return 1;
  } else if (bit_depth == 16) {
    return 2;
  } else if (bit_depth > 16) {
    return 4;
  }

  return 1;
}

uint8_t PixelAlign::GetChromaAlignment(const ChromaSubsampling& subsampling) {
  switch (subsampling) {
    case ChromaSubsampling::kSAMP_420:
    case ChromaSubsampling::kSAMP_422:
      return 2;
    case ChromaSubsampling::kSAMP_444:
    case ChromaSubsampling::kSAMP_400:
    default:
      return 1;
  }
}
