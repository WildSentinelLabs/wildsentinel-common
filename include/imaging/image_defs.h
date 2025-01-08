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

const std::string ColorSpaceToString(const ColorSpace& color_space);

std::ostream& operator<<(std::ostream& os, const ColorSpace& color_space);

const std::string ChromaSubsamplingToString(
    const ChromaSubsampling& subsampling);

std::ostream& operator<<(std::ostream& os,
                         const ChromaSubsampling& subsampling);
