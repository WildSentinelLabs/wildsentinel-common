#pragma once

#include "ws/imaging/chroma_subsampling.h"
#include "ws/imaging/color_space.h"
#include "ws/imaging/pixel/pixel_allowed_types.h"
#include "ws/imaging/pixel/pixel_format.h"
#include "ws/imaging/pixel/pixel_layout_flag.h"
#include "ws/span.h"

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

  constexpr bool HasAlpha() const;
};

// ============================================================================
// Implementation details for PixelFormatDetails
// ============================================================================

inline constexpr bool PixelFormatDetails::HasAlpha() const {
  return alpha_index != -1;
}
}  // namespace imaging
}  // namespace ws
