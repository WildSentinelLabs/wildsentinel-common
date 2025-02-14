#pragma once

#include <sstream>
#include <string>
namespace ws {
namespace imaging {

enum class ColorSpace {
  kGray,
  kSRgb,
  kCmyk,
  kSYcc,
  kSYcck,
  kEYcc,
  kUnknow,
  kUnsupported = -1,
};

const std::string ColorSpaceToString(const ColorSpace& color_space);

std::ostream& operator<<(std::ostream& os, const ColorSpace& color_space);
}  // namespace imaging
}  // namespace ws
