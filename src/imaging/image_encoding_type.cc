#include "imaging/image_encoding_type.h"
namespace ws {
namespace imaging {

const std::string ImageEncodingTypeToString(
    const ImageEncodingType& encoding_type) {
  switch (encoding_type) {
    case ImageEncodingType::kLossless:
      return "LOSSLESS";
    case ImageEncodingType::kLossy:
      return "LOSSY";
    case ImageEncodingType::kUnsupported:
      return "UNSUPPORTED";
    default:
      return "UNSUPPORTED";
  }
}

std::ostream& operator<<(std::ostream& os,
                         const ImageEncodingType& encoding_type) {
  os << ImageEncodingTypeToString(encoding_type);
  return os;
}
}  // namespace imaging
}  // namespace ws
