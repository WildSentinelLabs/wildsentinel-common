#include "imaging/image_compression_options.h"

namespace ws {
namespace imaging {

std::string ImageCompressionOptions::ToString() const {
  std::string result =
      Format("ImageCompressionOptions<{}>", ImageCompressionTypeToString(type));
  if (quality.has_value()) result += Format("(Quality: {})", quality.value());
  return result;
}

}  // namespace imaging
}  // namespace ws
