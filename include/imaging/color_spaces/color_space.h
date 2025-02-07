#pragma once

#include <sstream>
#include <string>

enum class ColorSpace {
  kGRAY,
  kRGB,
  kCMYK,
  kSYCC,
  kSYCCK,
  kEYCC,
  kUNKNOW,
  kUNSUPPORTED = -1,
};

const std::string ColorSpaceToString(const ColorSpace& color_space);

std::ostream& operator<<(std::ostream& os, const ColorSpace& color_space);
