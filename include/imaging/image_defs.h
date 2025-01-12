#pragma once

#include <sstream>
#include <string>

enum class ColorSpace {
  kGRAYSCALE,
  kRGB,
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
  kSAMP_400 = 3,  // GRAYSCALE
                  // OTHERS
  kSAMP_440 = 4,
  kSAMP_411 = 5,
  kUNSUPPORTED = -1,
};

enum class PixelFormat {
  kRGB,
  kRGBA,
  kBGR,
  kBGRA,
  kABGR,
  kCMYK,
  kCMYK_Alpha,
  kYUV444,
  kYUV422,
  kYUV420,
  kYUV411,
  kYUV440,
  kYUV400,
  kYA,
  kUYVYI,
  kYUY2I,
  kNV12I,
  kNV21I,
  kI420P,
  kI422P,
  kI444P,
  kYCbCr,
  kEYCC
};

enum class PixelType { kInterleaved, kPlanar };

const std::string ColorSpaceToString(const ColorSpace& color_space);

std::ostream& operator<<(std::ostream& os, const ColorSpace& color_space);

const std::string ChromaSubsamplingToString(
    const ChromaSubsampling& subsampling);

std::ostream& operator<<(std::ostream& os,
                         const ChromaSubsampling& subsampling);

const std::string PixelFormatToString(const PixelFormat& pixel_format);

std::ostream& operator<<(std::ostream& os, const PixelFormat& pixel_format);

const std::string PixelTypeToString(const PixelType& pixel_type);

std::ostream& operator<<(std::ostream& os, const PixelType& pixel_type);
