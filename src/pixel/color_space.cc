#include "pixel/color_space.h"

const std::string ColorSpaceToString(const ColorSpace& color_space) {
  switch (color_space) {
    case ColorSpace::kGRAY:
      return "GRAY";
    case ColorSpace::kRGB:
      return "RGB";
    case ColorSpace::kCMYK:
      return "CMYK";
    case ColorSpace::kSYCC:
      return "SYCC";
    case ColorSpace::kEYCC:
      return "E-YCC";
    case ColorSpace::kUNKNOW:
      return "UNKNOW";
    default:
      return "UNSUPPORTED";
  }
}

std::ostream& operator<<(std::ostream& os, const ColorSpace& color_space) {
  os << ColorSpaceToString(color_space);
  return os;
}
