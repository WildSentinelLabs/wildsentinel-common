#include "imaging/image_buffer_type.h"

namespace ws {
namespace imaging {
ImageBufferType DetermineImageBufferType(uint8_t bit_depth) {
  if (bit_depth >= 2 && bit_depth <= 8) {
    return ImageBufferType::kUInt8;
  } else if (bit_depth >= 9 && bit_depth <= 12) {
    return ImageBufferType::kInt16;
  } else if (bit_depth >= 13 && bit_depth <= 16) {
    return ImageBufferType::kUInt16;
  } else if (bit_depth >= 17 && bit_depth <= 24) {
    return ImageBufferType::kInt32;
  } else if (bit_depth >= 25 && bit_depth <= 32) {
    return ImageBufferType::kUInt32;
  } else {
    return ImageBufferType::kUnknown;
  }
}

std::string ImageBufferTypeToString(ImageBufferType type) {
  switch (type) {
    case ImageBufferType::kUInt8:
      return "UInt8";
    case ImageBufferType::kUInt16:
      return "UInt16";
    case ImageBufferType::kInt16:
      return "Int16";
    case ImageBufferType::kUInt32:
      return "UInt32";
    case ImageBufferType::kInt32:
      return "Int32";
    default:
      return "Unknown ";
  }
}
}  // namespace imaging
}  // namespace ws
