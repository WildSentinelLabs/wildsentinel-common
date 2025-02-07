#pragma once

#include <sstream>
#include <string>

enum class ColorSpace {
  kGray,
  kSRGB,
  kCMYK,
  kSYCC,
  kSYCCK,
  kEYCC,
  kUnknow,
  kUnsupported = -1,
};

const std::string ColorSpaceToString(const ColorSpace& color_space);

std::ostream& operator<<(std::ostream& os, const ColorSpace& color_space);
