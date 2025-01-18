#pragma once

#include <sstream>
#include <string>

enum class ColorSpace {
  kGRAY,
  kRGB,
  kCMYK,
  kSYCC,
  kEYCC,
  kUNKNOW,
  kUNSUPPORTED = -1,
};

enum class ChromaSubsampling {
  kSAMP_444 = 0,  // None
  kSAMP_422 = 1,  // Horizontal
  kSAMP_420 = 2,  // Both
  kSAMP_400 = 3,  // GRAYSCALE
                  // OTHERS
  kSAMP_440 = 4,
  kSAMP_411 = 5,
  kUNSUPPORTED = -1,
};

const std::string ColorSpaceToString(const ColorSpace& color_space);

std::ostream& operator<<(std::ostream& os, const ColorSpace& color_space);

const std::string ChromaSubsamplingToString(
    const ChromaSubsampling& subsampling);

std::ostream& operator<<(std::ostream& os,
                         const ChromaSubsampling& subsampling);
