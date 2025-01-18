#include "imaging/pixel_format.h"

const std::string PixelFormatToString(const PixelFormat& pixel_format) {
  switch (pixel_format) {
    case PixelFormat::kGRAY:
      return "kGRAY";
    case PixelFormat::kRGB:
      return "kRGB";
    case PixelFormat::kRGBA:
      return "kRGBA";
    case PixelFormat::kBGR:
      return "kBGR";
    case PixelFormat::kBGRA:
      return "kBGRA";
    case PixelFormat::kABGR:
      return "kABGR";
    case PixelFormat::kCMYK:
      return "kCMYK";
    case PixelFormat::kCMYKAlpha:
      return "kCMYKAlpha";
    case PixelFormat::kYA:
      return "kYA";
    case PixelFormat::kUYVY:
      return "kUYVY";
    case PixelFormat::kYUY2:
      return "kYUY2";
    case PixelFormat::kNV12:
      return "kNV12";
    case PixelFormat::kNV21:
      return "kNV21";
    case PixelFormat::kI400:
      return "kI400";
    case PixelFormat::kI411:
      return "kI411";
    case PixelFormat::kI420:
      return "kI420";
    case PixelFormat::kI422:
      return "kI422";
    case PixelFormat::kI440:
      return "kI440";
    case PixelFormat::kI444:
      return "kI444";
    default:
      return "UNSUPPORTED";
  }
}

std::ostream& operator<<(std::ostream& os, const PixelFormat& pixel_format) {
  os << PixelFormatToString(pixel_format);
  return os;
}
