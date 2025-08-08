#pragma once

#include "ws/array.h"
#include "ws/imaging/image.h"
#include "ws/imaging/image_traits.h"
#include "ws/imaging/pixel/pixel_format.h"
#include "ws/imaging/pixel/pixel_format_constraints.h"
#include "ws/status/status.h"
#include "ws/status/status_code.h"
#include "ws/status/status_or.h"

namespace ws {
namespace imaging {
template <ws::imaging::IsAllowedPixelNumericType T>
class ImageBufferExporter {
 public:
  using container_type = Array<T>;

  static StatusOr<container_type> ExportToInterleavedBuffer(
      const Image& image, ws::imaging::PixelFormat pixel_format);
  static StatusOr<container_type> ExportToPlanarBuffer(
      const Image& image, ws::imaging::PixelFormat pixel_format);
};
}  // namespace imaging
}  // namespace ws
