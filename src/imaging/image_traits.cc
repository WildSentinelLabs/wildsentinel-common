#include "imaging/image_traits.h"
namespace ws {
namespace imaging {

uint8_t ImageTraits::GetBitAlignment(const uint8_t& bit_depth) {
  if (bit_depth == 8) {
    return 1;
  } else if (bit_depth == 16) {
    return 2;
  } else if (bit_depth > 16) {
    return 4;
  }

  return 1;
}

uint8_t ImageTraits::GetChromaAlignment(const ChromaSubsampling& subsampling) {
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

bool ImageTraits::IsSigned(const uint8_t& bit_depth) {
  return bit_depth >= 9 && bit_depth <= 12;
}
}  // namespace imaging
}  // namespace ws
