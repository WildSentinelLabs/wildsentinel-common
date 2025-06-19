#pragma once

#include "imaging/chroma_subsampling.h"
#include "imaging/color_space.h"
#include "imaging/pixel/pixel_allowed_types.h"
#include "imaging/pixel/pixel_format.h"
#include "imaging/pixel/pixel_layout_flag.h"
#include "span.h"

namespace ws {
namespace imaging {

struct PixelFormatDetails {
  PixelFormat pixel_format;
  ColorSpace color_space;
  ChromaSubsampling chroma_subsampling;
  uint8_t num_components;
  ReadOnlySpan<uint8_t> components_order;
  int8_t alpha_index;
  PixelLayoutFlag layout;
  bool has_common_order;

  bool HasAlpha() const;
};

}  // namespace imaging
}  // namespace ws
