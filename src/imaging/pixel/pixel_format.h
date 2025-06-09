#pragma once
#include <cstdint>
#include <string>
namespace ws {
namespace imaging {
namespace pixel {

enum class PixelFormat : int8_t {
  kGray,
  kRgb,
  kRgba,
  kBgr,
  kBgra,
  kAbgr,
  kCmyk,
  kCmyka,
  kYa,
  kUyvy,
  kYuy2,
  kNv12,
  kNv21,
  kI400,
  kI411,
  kI420,
  kI422,
  kI440,
  kI441,
  kI444,
  kUnsupported = -1
};

const std::string PixelFormatToString(const PixelFormat& pixel_format);
}  // namespace pixel
}  // namespace imaging
}  // namespace ws
