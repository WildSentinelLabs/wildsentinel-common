#pragma once

#include "imaging/image_defs.h"
class PixelTraits {
 public:
  static uint8_t GetBitAlignment(const uint8_t& bit_depth);
  static uint8_t GetChromaAlignment(const ChromaSubsampling& subsampling);
  static bool IsSigned(const uint8_t& bit_depth);
};
