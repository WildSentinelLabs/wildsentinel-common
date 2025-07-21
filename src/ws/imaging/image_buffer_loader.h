#pragma once

#include "ws/imaging/image.h"
#include "ws/imaging/image_traits.h"
#include "ws/imaging/pixel/pixel_format.h"
#include "ws/imaging/pixel/pixel_format_constraints.h"
#include "ws/status/status_or.h"
namespace ws {
namespace imaging {
template <ws::imaging::IsAllowedPixelNumericType T>
class ImageBufferLoader {
 public:
  static StatusOr<Image> LoadFromInterleavedBuffer(
      ReadOnlySpan<T> buffer, uint32_t width, uint32_t height,
      uint8_t bit_depth,
      const ws::imaging::PixelFormatDetails* pixel_format_details);
  static StatusOr<Image> LoadFromInterleavedBuffer(
      ReadOnlySpan<T> buffer, uint32_t width, uint32_t height,
      uint8_t bit_depth, ws::imaging::PixelFormat pixel_format);
  static StatusOr<Image> LoadFromPlanarBuffer(
      ReadOnlySpan<T> buffer, uint32_t width, uint32_t height,
      uint8_t bit_depth,
      const ws::imaging::PixelFormatDetails* pixel_format_details);
  static StatusOr<Image> LoadFromPlanarBuffer(
      ReadOnlySpan<T> buffer, uint32_t width, uint32_t height,
      uint8_t bit_depth, ws::imaging::PixelFormat pixel_format);
};
}  // namespace imaging
}  // namespace ws
