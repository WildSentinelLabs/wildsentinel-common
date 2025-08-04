#include "ws/imaging/image_buffer_type.h"

namespace ws {
namespace imaging {

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
