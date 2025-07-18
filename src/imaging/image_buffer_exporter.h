#pragma once

#include "array.h"
#include "imaging/image.h"
#include "imaging/image_traits.h"
#include "imaging/pixel/pixel_format.h"
#include "imaging/pixel/pixel_format_constraints.h"
#include "status/status.h"
#include "status/status_code.h"
#include "status/status_or.h"

namespace ws {
namespace imaging {
template <ws::imaging::IsAllowedPixelNumericType T>
class ImageBufferExporter {
  static StatusOr<Array<T>> ExportToInterleavedBuffer(
      const Image& image, ws::imaging::PixelFormat pixel_format);
  static StatusOr<Array<T>> ExportToPlanarBuffer(
      const Image& image, ws::imaging::PixelFormat pixel_format);
};
}  // namespace imaging
}  // namespace ws
