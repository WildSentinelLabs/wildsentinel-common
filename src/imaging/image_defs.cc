#include "imaging/image_defs.h"

const std::string ColorSpaceToString(const ColorSpace& color_space) {
  switch (color_space) {
    case ColorSpace::kGRAYSCALE:
      return "Grayscale";
    case ColorSpace::kRGB:
      return "RGB";
    case ColorSpace::kCMYK:
      return "CMYK";
    case ColorSpace::kYUV:
      return "YUV";
    case ColorSpace::kYCbCr:
      return "YCbCr";
    case ColorSpace::kEYCC:
      return "E-YCC";
    default:
      return "Unsupported";
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
      return "Unsupported";
  }
}

std::ostream& operator<<(std::ostream& os,
                         const ChromaSubsampling& subsampling) {
  os << ChromaSubsamplingToString(subsampling);
  return os;
}

const std::string PixelFormatToString(const PixelFormat& pixel_format) {
  switch (pixel_format) {
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
    case PixelFormat::kCMYK_Alpha:
      return "kCMYK_Alpha";
    case PixelFormat::kYUV444:
      return "kYUV444";
    case PixelFormat::kYUV422:
      return "kYUV422";
    case PixelFormat::kYUV420:
      return "kYUV420";
    case PixelFormat::kYUV411:
      return "kYUV411";
    case PixelFormat::kYUV440:
      return "kYUV440";
    case PixelFormat::kYUV400:
      return "kYUV400";
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
    case PixelFormat::kI420P:
      return "kI420P";
    case PixelFormat::kI422P:
      return "kI422P";
    case PixelFormat::kI444P:
      return "kI444P";
    case PixelFormat::kYCbCr:
      return "kYCbCr";
    case PixelFormat::kEYCC:
      return "kEYC";
    default:
      return "Unsupported";
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
      return "Unsupported";
  }
}

std::ostream& operator<<(std::ostream& os, const PixelType& pixel_type) {
  os << PixelTypeToString(pixel_type);
  return os;
}
