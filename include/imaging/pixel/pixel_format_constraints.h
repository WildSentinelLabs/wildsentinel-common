#pragma once

#include "imaging/chroma_subsampling.h"
#include "imaging/color_space.h"
#include "imaging/pixel/pixel_allowed_types.h"
#include "imaging/pixel/pixel_format.h"
#include "imaging/pixel/pixel_format_details.h"

namespace ws {
namespace imaging {
namespace pixel {

// Component orders for each PixelFormat
// Rgb and Rgba formats
static constexpr uint8_t kRgbOrder[3] = {0, 1, 2};      // R, G, B
static constexpr uint8_t kRgbaOrder[4] = {0, 1, 2, 3};  // R, G, B, A (Alpha)
static constexpr uint8_t kBgrOrder[3] = {2, 1, 0};      // B, G, R
static constexpr uint8_t kBgrAOrder[4] = {2, 1, 0, 3};  // B, G, R, A (Alpha)
static constexpr uint8_t kABgrOrder[4] = {3, 2, 1, 0};  // A, B, G, R
static constexpr uint8_t kCmykOrder[4] = {0, 1, 2, 3};  // C, M, Y, K
static constexpr uint8_t kCmykaOrder[5] = {0, 1, 2, 3, 4};  // C, M, Y, K, Alpha

// Grayscale and Ya formats
static constexpr uint8_t kGrayOrder[1] = {0};   // Grayscale (only Y)
static constexpr uint8_t kYaOrder[2] = {0, 1};  // Y, A (Alpha)

// YUV and YCbCr formats, standard orders for these
static constexpr uint8_t kUyvyOrder[4] = {1, 0, 0, 2};  // Y, U, Y, V
static constexpr uint8_t kYuy2Order[4] = {0, 1, 0, 2};  // Y, U, Y, V
static constexpr uint8_t kNv12Order[3] = {0, 1, 2};     // Y, Cb, Cr
static constexpr uint8_t kNv21Order[3] = {0, 2, 1};     // Y, Cr, Cb
static constexpr uint8_t kI420Order[3] = {0, 1, 2};     // Y, Cb, Cr (Planar)
static constexpr uint8_t kI422Order[3] = {0, 1, 2};     // Y, Cb, Cr (Planar)
static constexpr uint8_t kI444Order[3] = {0, 1, 2};     // Y, Cb, Cr (Planar)
static constexpr uint8_t kI400Order[1] = {0};           // Y, Cb, Cr (planar)
static constexpr uint8_t kI411Order[3] = {0, 1, 2};     // Y, Cb, Cr (Planar)
static constexpr uint8_t kI441Order[3] = {0, 1, 2};     // Y, Cb, Cr (Planar)
static constexpr uint8_t kI440Order[3] = {0, 1, 2};     // Y, Cb, Cr (Planar)

class PixelFormatConstraints {
 public:
  // Pixel format details
  static constexpr PixelFormatDetails kRgb = {PixelFormat::kRgb,
                                              ColorSpace::kSRgb,
                                              ChromaSubsampling::kSamp444,
                                              3,
                                              kRgbOrder,
                                              3,
                                              -1};

  static constexpr PixelFormatDetails kRgba = {PixelFormat::kRgba,
                                               ColorSpace::kSRgb,
                                               ChromaSubsampling::kSamp444,
                                               4,
                                               kRgbaOrder,
                                               4,
                                               3};

  static constexpr PixelFormatDetails kBgr = {PixelFormat::kRgb,
                                              ColorSpace::kSRgb,
                                              ChromaSubsampling::kSamp444,
                                              3,
                                              kBgrOrder,
                                              3,
                                              -1};

  static constexpr PixelFormatDetails kBgrA = {PixelFormat::kBgra,
                                               ColorSpace::kSRgb,
                                               ChromaSubsampling::kSamp444,
                                               4,
                                               kBgrAOrder,
                                               4,
                                               3};

  static constexpr PixelFormatDetails kABgr = {PixelFormat::kAbgr,
                                               ColorSpace::kSRgb,
                                               ChromaSubsampling::kSamp444,
                                               4,
                                               kABgrOrder,
                                               4,
                                               3};

  static constexpr PixelFormatDetails kCmyk = {PixelFormat::kCmyk,
                                               ColorSpace::kCmyk,
                                               ChromaSubsampling::kSamp444,
                                               4,
                                               kCmykOrder,
                                               4,
                                               -1};

  static constexpr PixelFormatDetails kCmyka = {PixelFormat::kCmyka,
                                                ColorSpace::kCmyk,
                                                ChromaSubsampling::kSamp444,
                                                5,
                                                kCmykaOrder,
                                                5,
                                                4};

  static constexpr PixelFormatDetails kGray = {PixelFormat::kGray,
                                               ColorSpace::kGray,
                                               ChromaSubsampling::kSamp400,
                                               1,
                                               kGrayOrder,
                                               1,
                                               -1};

  static constexpr PixelFormatDetails kYa = {PixelFormat::kYa,
                                             ColorSpace::kSYcc,
                                             ChromaSubsampling::kSamp400,
                                             2,
                                             kYaOrder,
                                             2,
                                             1};

  static constexpr PixelFormatDetails kUyvy = {PixelFormat::kUyvy,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp422,
                                               3,
                                               kUyvyOrder,
                                               4,
                                               -1};

  static constexpr PixelFormatDetails kYuy2 = {PixelFormat::kYuy2,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp422,
                                               3,
                                               kYuy2Order,
                                               4,
                                               -1};

  static constexpr PixelFormatDetails kNV12 = {PixelFormat::kNv12,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp420,
                                               3,
                                               kNv12Order,
                                               4,
                                               -1};

  static constexpr PixelFormatDetails kNv21 = {PixelFormat::kNv21,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp420,
                                               3,
                                               kNv21Order,
                                               4,
                                               -1};

  static constexpr PixelFormatDetails kI400 = {PixelFormat::kI400,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp400,
                                               1,
                                               kI400Order,
                                               1,
                                               -1};

  static constexpr PixelFormatDetails kI411 = {PixelFormat::kI411,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp411,
                                               3,
                                               kI411Order,
                                               3,
                                               -1};

  static constexpr PixelFormatDetails kI420 = {PixelFormat::kI420,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp420,
                                               3,
                                               kI420Order,
                                               3,
                                               -1};

  static constexpr PixelFormatDetails kI422 = {PixelFormat::kI422,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp422,
                                               3,
                                               kI422Order,
                                               3,
                                               -1};

  static constexpr PixelFormatDetails kI440 = {PixelFormat::kI440,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp440,
                                               3,
                                               kI440Order,
                                               3,
                                               -1};

  static constexpr PixelFormatDetails kI441 = {PixelFormat::kI441,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp441,
                                               3,
                                               kI441Order,
                                               3,
                                               -1};

  static constexpr PixelFormatDetails kI444 = {PixelFormat::kI444,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp444,
                                               3,
                                               kI444Order,
                                               3,
                                               -1};

  // Planar formats
  static constexpr const PixelFormatDetails* kPlanarFormats[] = {
      &kI411,  // YUV 4:1:1 (Planar)
      &kI420,  // YUV 4:2:0 (Planar)
      &kI422,  // YUV 4:2:2 (Planar)
      &kI440,  // YUV 4:4:0 (Planar)
      &kI441,  // YUV 4:4:0 (Planar)
      &kI444,  // YUV 4:4:4 (Planar)
      &kI400,  // Grayscale (Planar)
      &kGray,  // Grayscale (just Y component)
      &kYa,    // Grayscale with Alpha
      // Formats that can be planar (supported as planar)
      &kRgb,    // Rgb (Interleaved)
      &kRgba,   // Rgba (Interleaved)
      &kBgrA,   // BgrA (Interleaved)
      &kABgr,   // ABgr (Interleaved)
      &kCmyk,   // Cmyk (Interleaved)
      &kCmyka,  // Cmyk with Alpha (Interleaved)
  };

  // Interleaved formats
  static constexpr const PixelFormatDetails* kInterleavedFormats[] = {
      &kUyvy,  // YUV 4:2:2 (Interleaved)
      &kYuy2,  // YUV 4:2:2 (Interleaved)
      &kNV12,  // YUV 4:2:0 (Interleaved)
      &kNv21,  // YUV 4:2:0 (Interleaved)
      // Formats that can be interleaved (supported as interleaved)
      &kRgb,    // Rgb (Interleaved)
      &kRgba,   // Rgba (Interleaved)
      &kBgrA,   // BgrA (Interleaved)
      &kABgr,   // ABgr (Interleaved)
      &kCmyk,   // Cmyk (Interleaved)
      &kCmyka,  // Cmyk with Alpha (Interleaved)
      &kI400,   // Grayscale (Planar)
      &kGray,   // Grayscale (just Y component)
      &kYa,     // Grayscale with Alpha
  };

  static const PixelFormatDetails* GetPlanarFormat(
      const PixelFormat& pixel_format);

  static const PixelFormatDetails** GetPlanarFormats(
      const ColorSpace& color_space,
      const ChromaSubsampling& chroma_subsampling,
      const uint8_t& num_components, const bool& has_alpha, uint8_t& size);

  static const PixelFormatDetails* GetInterleavedFormat(
      const PixelFormat& pixel_format);

  static const PixelFormatDetails** GetInterleavedFormats(
      const ColorSpace& color_space,
      const ChromaSubsampling& chroma_subsampling,
      const uint8_t& num_components, const bool& has_alpha, uint8_t& size);

  static bool GetComponentDimensions(uint32_t width, uint32_t height,
                                     uint8_t num_components,
                                     ChromaSubsampling chroma_subsampling,
                                     bool has_alpha, uint32_t*& comps_width,
                                     uint32_t*& comps_height);
};

// TODO: Enhace pixel_format management and structures if needed
}  // namespace pixel
}  // namespace imaging
}  // namespace ws
