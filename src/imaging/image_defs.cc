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
