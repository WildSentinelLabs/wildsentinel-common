#include "ws/imaging/color_space.h"
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
}  // namespace imaging
}  // namespace ws
