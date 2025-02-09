#include "imaging/color_space.h"

const std::string ColorSpaceToString(const ColorSpace& color_space) {
  switch (color_space) {
    case ColorSpace::kGray:
      return "Gray";
    case ColorSpace::kSRGB:
      return "SRGB";
    case ColorSpace::kCMYK:
      return "CMYK";
    case ColorSpace::kSYCC:
      return "SYCC";
    case ColorSpace::kEYCC:
      return "E-YCC";
    case ColorSpace::kSYCCK:
      return "SYCCK";
    case ColorSpace::kUnknow:
      return "Unknow";
    default:
      return "Unsupported";
  }
}

std::ostream& operator<<(std::ostream& os, const ColorSpace& color_space) {
  os << ColorSpaceToString(color_space);
  return os;
}
