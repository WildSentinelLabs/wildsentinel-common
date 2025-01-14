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
