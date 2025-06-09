#pragma once
#include "imaging/image.h"
#include "imaging/image_traits.h"
#include "imaging/pixel/pixel_format.h"
#include "imaging/pixel/pixel_format_constraints.h"
namespace ws {
namespace imaging {
template <ws::imaging::pixel::IsAllowedPixelNumericType T>
class ImageBufferLoader {
 public:
  static Image LoadFromInterleavedBuffer(
      ReadOnlySpan<T> buffer, uint32_t width, uint32_t height,
      uint8_t bit_depth,
      const ws::imaging::pixel::PixelFormatDetails* pixel_format_details);

  static Image LoadFromInterleavedBuffer(
      ReadOnlySpan<T> buffer, uint32_t width, uint32_t height,
      uint8_t bit_depth, ws::imaging::pixel::PixelFormat pixel_format);

  static Image LoadFromPlanarBuffer(
      ReadOnlySpan<T> buffer, uint32_t width, uint32_t height,
      uint8_t bit_depth,
      const ws::imaging::pixel::PixelFormatDetails* pixel_format_details);

  static Image LoadFromPlanarBuffer(
      ReadOnlySpan<T> buffer, uint32_t width, uint32_t height,
      uint8_t bit_depth, ws::imaging::pixel::PixelFormat pixel_format);
};
}  // namespace imaging
}  // namespace ws
