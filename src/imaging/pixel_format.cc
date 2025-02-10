#include "imaging/pixel_format.h"

const std::string PixelFormatToString(const PixelFormat& pixel_format) {
  switch (pixel_format) {
    case PixelFormat::kGray:
      return "GRAY";
    case PixelFormat::kRGB:
      return "RGB";
    case PixelFormat::kRGBA:
      return "RGBA";
    case PixelFormat::kBGR:
      return "BGR";
    case PixelFormat::kBGRA:
      return "BGRA";
    case PixelFormat::kABGR:
      return "ABGR";
    case PixelFormat::kCMYK:
      return "CMYK";
    case PixelFormat::kCMYKA:
      return "CMYKA";
    case PixelFormat::kYA:
      return "YA";
    case PixelFormat::kUYVY:
      return "UYVY";
    case PixelFormat::kYUY2:
      return "YUY2";
    case PixelFormat::kNV12:
      return "NV12";
    case PixelFormat::kNV21:
      return "NV21";
    case PixelFormat::kI400:
      return "I400";
    case PixelFormat::kI411:
      return "I411";
    case PixelFormat::kI420:
      return "I420";
    case PixelFormat::kI422:
      return "I422";
    case PixelFormat::kI440:
      return "I440";
    case PixelFormat::kI444:
      return "I444";
    default:
      return "Unsupported";
  }
}

std::ostream& operator<<(std::ostream& os, const PixelFormat& pixel_format) {
  os << PixelFormatToString(pixel_format);
  return os;
}
