#include "imaging/color_space.h"
namespace ws {
namespace imaging {
const std::string ColorSpaceToString(const ColorSpace& color_space) {
  switch (color_space) {
    case ColorSpace::kGray:
      return "GRAY";
    case ColorSpace::kSRgb:
      return "SRGB";
    case ColorSpace::kCmyk:
      return "CMYK";
    case ColorSpace::kSYcc:
      return "SYCC";
    case ColorSpace::kEYcc:
      return "E-YCC";
    case ColorSpace::kSYcck:
      return "SYCCK";
    case ColorSpace::kUnknow:
      return "UNKNOW";
    default:
      return "UNSUPPORTED";
  }
}

std::ostream& operator<<(std::ostream& os, const ColorSpace& color_space) {
  os << ColorSpaceToString(color_space);
  return os;
}
}  // namespace imaging
}  // namespace ws
