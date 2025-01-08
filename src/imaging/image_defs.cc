#include "imaging/image_defs.h"

const std::string ColorSpaceToString(const ColorSpace& color_space) {
  switch (color_space) {
    case ColorSpace::kGRAYSCALE:
      return "Grayscale";
    case ColorSpace::kRGB:
      return "RGB";
    case ColorSpace::kBGR:
      return "BGR";
    case ColorSpace::kRGBA:
      return "RGBA";
    case ColorSpace::kBGRA:
      return "BGRA";
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
