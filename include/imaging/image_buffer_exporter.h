#pragma once
#include "imaging/image.h"
#include "imaging/image_traits.h"
#include "imaging/pixel/pixel_format.h"
#include "imaging/pixel/pixel_format_constraints.h"

namespace ws {
namespace imaging {

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
class ImageBufferExporter {
 public:
  static T* ExportToInterleavedBuffer(
      const Image& image, const ws::imaging::pixel::PixelFormat& pixel_format);

  static T* ExportToPlanarBuffer(
      const Image& image, const ws::imaging::pixel::PixelFormat& pixel_format);
};
}  // namespace imaging
}  // namespace ws
