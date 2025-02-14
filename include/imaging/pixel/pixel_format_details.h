#pragma once

#include "imaging/chroma_subsampling.h"
#include "imaging/color_space.h"
#include "imaging/pixel/pixel_allowed_types.h"
#include "imaging/pixel/pixel_format.h"

namespace ws {
namespace imaging {
namespace pixel {
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
}  // namespace pixel
}  // namespace imaging
}  // namespace ws
