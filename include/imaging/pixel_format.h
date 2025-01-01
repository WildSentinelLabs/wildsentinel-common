#pragma once
#include "imaging/image_defs.h"

struct PixelFormat {
  ColorSpace color_space;
  uint8_t num_components;
  ChromaSubsampling subsampling;

  PixelFormat(const ColorSpace color_space, const uint8_t num_components,
              const ChromaSubsampling subsampling);

  PixelFormat(const ColorSpace color_space, const uint8_t num_components);

  ~PixelFormat() = default;

  bool IsValid() const;

  std::string ToString() const;

  bool operator==(const PixelFormat& other) const;

  bool operator!=(const PixelFormat& other) const;
};

std::ostream& operator<<(std::ostream& os, const PixelFormat& format);

// Pixel Formats
const PixelFormat PF_GRAYSCALE =
    PixelFormat(ColorSpace::kGRAYSCALE, 1, ChromaSubsampling::kSAMP_400);
const PixelFormat PF_YUV400 =
    PixelFormat(ColorSpace::kYUV, 1, ChromaSubsampling::kSAMP_400);
const PixelFormat PF_EYCC400 =
    PixelFormat(ColorSpace::kEYCC, 1, ChromaSubsampling::kSAMP_400);
const PixelFormat PF_YA =
    PixelFormat(ColorSpace::kGRAYSCALE, 2, ChromaSubsampling::kSAMP_400);
const PixelFormat PF_YCbCr =
    PixelFormat(ColorSpace::kYUV, 3, ChromaSubsampling::kSAMP_444);
const PixelFormat PF_SRGB =
    PixelFormat(ColorSpace::kRGB, 3, ChromaSubsampling::kSAMP_444);
const PixelFormat PF_SBGR =
    PixelFormat(ColorSpace::kBGR, 3, ChromaSubsampling::kSAMP_444);
const PixelFormat PF_YUV420 =
    PixelFormat(ColorSpace::kYUV, 3, ChromaSubsampling::kSAMP_420);
const PixelFormat PF_YUV422 =
    PixelFormat(ColorSpace::kYUV, 3, ChromaSubsampling::kSAMP_422);
const PixelFormat PF_YUV444 =
    PixelFormat(ColorSpace::kYUV, 3, ChromaSubsampling::kSAMP_444);
const PixelFormat PF_EYCC420 =
    PixelFormat(ColorSpace::kEYCC, 3, ChromaSubsampling::kSAMP_420);
const PixelFormat PF_EYCC422 =
    PixelFormat(ColorSpace::kEYCC, 3, ChromaSubsampling::kSAMP_422);
const PixelFormat PF_EYCC444 =
    PixelFormat(ColorSpace::kEYCC, 3, ChromaSubsampling::kSAMP_444);
const PixelFormat PF_CMYK =
    PixelFormat(ColorSpace::kCMYK, 4, ChromaSubsampling::kSAMP_444);
const PixelFormat PF_SRGBA =
    PixelFormat(ColorSpace::kRGBA, 4, ChromaSubsampling::kSAMP_444);
const PixelFormat PF_SBGRA =
    PixelFormat(ColorSpace::kBGRA, 4, ChromaSubsampling::kSAMP_444);
const PixelFormat PF_CMYK_Alpha =
    PixelFormat(ColorSpace::kCMYK, 5, ChromaSubsampling::kSAMP_444);
