#pragma once
#include <string>

enum class PixelFormat {
  kGRAY,
  kRGB,
  kRGBA,
  kBGR,
  kBGRA,
  kABGR,
  kCMYK,
  kCMYKA,
  kYA,
  kUYVY,
  kYUY2,
  kNV12,
  kNV21,
  kI400,
  kI411,
  kI420,
  kI422,
  kI440,
  kI441,
  kI444,
  kUNSUPPORTED = -1
};

const std::string PixelFormatToString(const PixelFormat& pixel_format);

std::ostream& operator<<(std::ostream& os, const PixelFormat& pixel_format);
