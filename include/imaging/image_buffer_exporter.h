#pragma once
#include "array.h"
#include "imaging/image.h"
#include "imaging/image_traits.h"
#include "imaging/pixel/pixel_format.h"
#include "imaging/pixel/pixel_format_constraints.h"

namespace ws {
namespace imaging {

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
class ImageBufferExporter {
 public:
  static Array<T> ExportToInterleavedBuffer(
      const Image& image, ws::imaging::pixel::PixelFormat pixel_format);

  static Array<T> ExportToPlanarBuffer(
      const Image& image, ws::imaging::pixel::PixelFormat pixel_format);
};
}  // namespace imaging
}  // namespace ws
