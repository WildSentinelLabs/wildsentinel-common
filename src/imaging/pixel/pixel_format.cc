#include "imaging/pixel/pixel_format.h"
namespace ws {
namespace imaging {
namespace pixel {

const std::string PixelFormatToString(const PixelFormat& pixel_format) {
  switch (pixel_format) {
    case PixelFormat::kGray:
      return "GRAY";
    case PixelFormat::kRgb:
      return "RGB";
    case PixelFormat::kRgba:
      return "RGBA";
    case PixelFormat::kBgr:
      return "BGR";
    case PixelFormat::kBgra:
      return "BGRA";
    case PixelFormat::kAbgr:
      return "ABGR";
    case PixelFormat::kCmyk:
      return "CMYK";
    case PixelFormat::kCmyka:
      return "CMYKA";
    case PixelFormat::kYa:
      return "YA";
    case PixelFormat::kUyvy:
      return "UYVY";
    case PixelFormat::kYuy2:
      return "YUY2";
    case PixelFormat::kNv12:
      return "NV12";
    case PixelFormat::kNv21:
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
}  // namespace pixel
}  // namespace imaging
}  // namespace ws
