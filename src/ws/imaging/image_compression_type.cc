#include "ws/imaging/image_compression_type.h"
namespace ws {
namespace imaging {
const std::string ImageCompressionTypeToString(
    const ImageCompressionType& encoding_type) {
  switch (encoding_type) {
    case ImageCompressionType::kLossless:
      return "LOSSLESS";
    case ImageCompressionType::kLossy:
      return "LOSSY";
    case ImageCompressionType::kUnsupported:
      return "UNSUPPORTED";
    default:
      return "UNSUPPORTED";
  }
}
}  // namespace imaging
}  // namespace ws
