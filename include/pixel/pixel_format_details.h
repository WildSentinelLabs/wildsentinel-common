#pragma once

#include "pixel/chroma_subsampling.h"
#include "pixel/color_space.h"
#include "pixel_format.h"

struct PixelFormatDetails {
  const PixelFormat pixel_format;
  ColorSpace color_space;
  ChromaSubsampling chroma_subsampling;
  uint8_t num_components;
  const uint8_t* components_order_sequence;
  const uint8_t components_order_size;
  int8_t alpha_index;

  bool HasAlpha() const;
};
