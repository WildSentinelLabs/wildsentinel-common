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
      return "e-YCC";
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

const std::string ImageFormatToString(const ImageFormat& format) {
  switch (format) {
    case ImageFormat::kJPEG:
      return "JPEG";
    case ImageFormat::kJP2:
      return "JP2";
    case ImageFormat::kHTJ2K:
      return "HTJ2K";
    case ImageFormat::kUNSUPPORTED:
      return "UNSUPPORTED";
    default:
      return "UNSUPPORTED";
  }
}

std::ostream& operator<<(std::ostream& os, const ImageFormat& format) {
  os << ImageFormatToString(format);
  return os;
}

const std::string EncodingModeToString(const EncodingMode& mode) {
  switch (mode) {
    case EncodingMode::kLOSSLESS:
      return "LOSSLESS";
    case EncodingMode::kLOSSY:
      return "LOSSY";
    case EncodingMode::kUNSUPPORTED:
      return "UNSUPPORTED";
    default:
      return "UNSUPPORTED";
  }
}

std::ostream& operator<<(std::ostream& os, const EncodingMode& mode) {
  os << EncodingModeToString(mode);
  return os;
}

const std::string ImageTagToString(const ImageTag& tag) {
  switch (tag) {
    case ImageTag::kSUB_SAMP:
      return "SUB_SAMP";
    case ImageTag::kX_DENSITY:
      return "X_DENSITY";
    case ImageTag::kY_DENSITY:
      return "Y_DENSITY";
    case ImageTag::kDENSITY_UNITS:
      return "DENSITY_UNITS";
    case ImageTag::kBOTTOMUP:
      return "BOTTOMUP";
    case ImageTag::kSTOP_ON_WARNING:
      return "STOP_ON_WARNING";
    case ImageTag::kFAST_DCT:
      return "FAST_DCT";
    case ImageTag::kFAST_UP_SAMPLE:
      return "FAST_UP_SAMPLE";
    case ImageTag::kOPTIMIZE:
      return "OPTIMIZE";
    case ImageTag::kNUMTHREADS:
      return "NUMTHREADS";
    default:
      return "UNKNOWN";
  }
}

std::ostream& operator<<(std::ostream& os, const ImageTag& tag) {
  os << ImageTagToString(tag);
  return os;
}
