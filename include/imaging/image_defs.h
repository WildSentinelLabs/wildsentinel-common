#pragma once

#include <sstream>
#include <string>

enum class ColorSpace {
  kGRAYSCALE,
  kRGB,
  kBGR,
  kRGBA,
  kBGRA,
  kCMYK,
  kYUV,
  kYCbCr,
  kEYCC,
  kUNSUPPORTED = -1,
};

enum class ChromaSubsampling {
  kSAMP_444 = 0,  // None
  kSAMP_422 = 1,  // Horizontal
  kSAMP_420 = 2,  // Both
  kSAMP_400 = 3,
  kUNSUPPORTED = -1,
};

enum class ImageFormat : short int {
  kRAW,
  kJPEG,
  kJP2,
  kJ2K,
  kHTJ2K,
  kUNSUPPORTED = -1,
};

enum class ImageTag : long int {
  kSUB_SAMP,
  kX_DENSITY,
  kY_DENSITY,
  kDENSITY_UNITS,
  kBOTTOMUP,
  kSTOP_ON_WARNING,
  kFAST_DCT,
  kFAST_UP_SAMPLE,
  kOPTIMIZE,
  kNUMTHREADS,
  kFORCEYUV,
};

enum class RAWCodec : int {
  kRGB = 0,
};

enum class EncodingMode : int {
  kLOSSY = 0,
  kLOSSLESS = 1,
  kUNSUPPORTED = -1,
};

const std::string ColorSpaceToString(const ColorSpace& color_space);

std::ostream& operator<<(std::ostream& os, const ColorSpace& color_space);

const std::string ChromaSubsamplingToString(
    const ChromaSubsampling& subsampling);

std::ostream& operator<<(std::ostream& os,
                         const ChromaSubsampling& subsampling);

const std::string ImageFormatToString(const ImageFormat& format);

std::ostream& operator<<(std::ostream& os, const ImageFormat& format);

const std::string EncodingModeToString(const EncodingMode& mode);

std::ostream& operator<<(std::ostream& os, const EncodingMode& mode);

const std::string ImageTagToString(const ImageTag& tag);

std::ostream& operator<<(std::ostream& os, const ImageTag& context);
