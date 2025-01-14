#include "imaging/image_defs.h"

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

const std::string ChromaSubsamplingToString(
    const ChromaSubsampling& subsampling) {
  switch (subsampling) {
    case ChromaSubsampling::kSAMP_444:
      return "SAMP_444";
    case ChromaSubsampling::kSAMP_422:
      return "SAMP_422";
    case ChromaSubsampling::kSAMP_420:
      return "SAMP_420";
    case ChromaSubsampling::kSAMP_400:
      return "SAMP_400";
    default:
      return "UNSUPPORTED";
  }
}

std::ostream& operator<<(std::ostream& os,
                         const ChromaSubsampling& subsampling) {
  os << ChromaSubsamplingToString(subsampling);
  return os;
}

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
    case PixelFormat::kUYVYI:
      return "kUYVYI";
    case PixelFormat::kYUY2I:
      return "kYUY2I";
    case PixelFormat::kNV12I:
      return "kNV12I";
    case PixelFormat::kNV21I:
      return "kNV21I";
    case PixelFormat::kI400P:
      return "kI400P";
    case PixelFormat::kI411P:
      return "kI411P";
    case PixelFormat::kI420P:
      return "kI420P";
    case PixelFormat::kI422P:
      return "kI422P";
    case PixelFormat::kI440P:
      return "kI440P";
    case PixelFormat::kI444P:
      return "kI444P";
    default:
      return "UNSUPPORTED";
  }
}

std::ostream& operator<<(std::ostream& os, const PixelFormat& pixel_format) {
  os << PixelFormatToString(pixel_format);
  return os;
}

const std::string PixelTypeToString(const PixelType& pixel_type) {
  switch (pixel_type) {
    case PixelType::kInterleaved:
      return "Interleaved";
    case PixelType::kPlanar:
      return "Planar";
    default:
      return "UNSUPPORTED";
  }
}

std::ostream& operator<<(std::ostream& os, const PixelType& pixel_type) {
  os << PixelTypeToString(pixel_type);
  return os;
}
