#include "imaging/image_defs.h"
#include "imaging/pixel_format_details.h"

// Component orders for each PixelFormat
// RGB and RGBA formats
static constexpr uint8_t kRGBOrder[3] = {0, 1, 2};      // R, G, B
static constexpr uint8_t kRGBAOrder[4] = {0, 1, 2, 3};  // R, G, B, A (Alpha)
static constexpr uint8_t kBGROrder[3] = {2, 1, 0};      // B, G, R
static constexpr uint8_t kBGRAOrder[4] = {2, 1, 0, 3};  // B, G, R, A (Alpha)
static constexpr uint8_t kABGROrder[4] = {3, 2, 1, 0};  // A, B, G, R
static constexpr uint8_t kCMYKOrder[4] = {0, 1, 2, 3};  // C, M, Y, K
static constexpr uint8_t kCMYKAlphaOrder[5] = {0, 1, 2, 3,
                                               4};  // C, M, Y, K, Alpha

// Grayscale and YA formats
static constexpr uint8_t kGRAYOrder[1] = {0};   // Grayscale (only Y)
static constexpr uint8_t kYAOrder[2] = {0, 1};  // Y, A (Alpha)

// YUV and YCbCr formats, standard orders for these
static constexpr uint8_t kUYVYOrder[4] = {1, 0, 0, 2};  // Y, U, Y, V
static constexpr uint8_t kYUY2Order[4] = {0, 1, 0, 2};  // Y, U, Y, V
static constexpr uint8_t kNV12Order[3] = {0, 1, 2};     // Y, Cb, Cr
static constexpr uint8_t kNV21Order[3] = {0, 2, 1};     // Y, Cr, Cb
static constexpr uint8_t kI420Order[3] = {0, 1, 2};     // Y, Cb, Cr (Planar)
static constexpr uint8_t kI422Order[3] = {0, 1, 2};     // Y, Cb, Cr (Planar)
static constexpr uint8_t kI444Order[3] = {0, 1, 2};     // Y, Cb, Cr (Planar)
static constexpr uint8_t kI400Order[1] = {0};           // Y, Cb, Cr (planar)
static constexpr uint8_t kI411Order[3] = {0, 1, 2};     // Y, Cb, Cr (Planar)
static constexpr uint8_t kI440Order[3] = {0, 1, 2};     // Y, Cb, Cr (Planar)

class PixelFormatConstraints {
 public:
  // Pixel format details
  static constexpr PixelFormatDetails kRGB = {PixelFormat::kRGB,
                                              ColorSpace::kRGB,
                                              ChromaSubsampling::kSAMP_444,
                                              3,
                                              kRGBOrder,
                                              3,
                                              -1};

  static constexpr PixelFormatDetails kRGBA = {PixelFormat::kRGBA,
                                               ColorSpace::kRGB,
                                               ChromaSubsampling::kSAMP_444,
                                               3,
                                               kRGBAOrder,
                                               4,
                                               3};

  static constexpr PixelFormatDetails kBGR = {PixelFormat::kRGB,
                                              ColorSpace::kRGB,
                                              ChromaSubsampling::kSAMP_444,
                                              3,
                                              kBGROrder,
                                              3,
                                              -1};

  static constexpr PixelFormatDetails kBGRA = {PixelFormat::kBGRA,
                                               ColorSpace::kRGB,
                                               ChromaSubsampling::kSAMP_444,
                                               3,
                                               kBGRAOrder,
                                               4,
                                               3};

  static constexpr PixelFormatDetails kABGR = {PixelFormat::kABGR,
                                               ColorSpace::kRGB,
                                               ChromaSubsampling::kSAMP_444,
                                               3,
                                               kABGROrder,
                                               4,
                                               3};

  static constexpr PixelFormatDetails kCMYK = {PixelFormat::kCMYK,
                                               ColorSpace::kCMYK,
                                               ChromaSubsampling::kSAMP_444,
                                               4,
                                               kCMYKOrder,
                                               4,
                                               -1};

  static constexpr PixelFormatDetails kCMYKA = {PixelFormat::kCMYKAlpha,
                                                ColorSpace::kCMYK,
                                                ChromaSubsampling::kSAMP_444,
                                                4,
                                                kCMYKAlphaOrder,
                                                5,
                                                4};

  static constexpr PixelFormatDetails kGRAY = {PixelFormat::kGRAY,
                                               ColorSpace::kGRAY,
                                               ChromaSubsampling::kSAMP_400,
                                               1,
                                               kGRAYOrder,
                                               1,
                                               -1};

  static constexpr PixelFormatDetails kYA = {PixelFormat::kYA,
                                             ColorSpace::kGRAY,
                                             ChromaSubsampling::kSAMP_400,
                                             1,
                                             kYAOrder,
                                             2,
                                             1};

  static constexpr PixelFormatDetails kUYVY = {PixelFormat::kUYVY,
                                               ColorSpace::kSYCC,
                                               ChromaSubsampling::kSAMP_422,
                                               3,
                                               kUYVYOrder,
                                               4,
                                               -1};

  static constexpr PixelFormatDetails kYUY2 = {PixelFormat::kYUY2,
                                               ColorSpace::kSYCC,
                                               ChromaSubsampling::kSAMP_422,
                                               3,
                                               kYUY2Order,
                                               4,
                                               -1};

  static constexpr PixelFormatDetails kNV12 = {PixelFormat::kNV12,
                                               ColorSpace::kSYCC,
                                               ChromaSubsampling::kSAMP_420,
                                               3,
                                               kNV12Order,
                                               4,
                                               -1};

  static constexpr PixelFormatDetails kNV21 = {PixelFormat::kNV21,
                                               ColorSpace::kSYCC,
                                               ChromaSubsampling::kSAMP_420,
                                               3,
                                               kNV21Order,
                                               4,
                                               -1};

  static constexpr PixelFormatDetails kI400 = {PixelFormat::kI400,
                                               ColorSpace::kGRAY,
                                               ChromaSubsampling::kSAMP_400,
                                               1,
                                               kI400Order,
                                               1,
                                               -1};

  static constexpr PixelFormatDetails kI411 = {PixelFormat::kI411,
                                               ColorSpace::kSYCC,
                                               ChromaSubsampling::kSAMP_411,
                                               3,
                                               kI411Order,
                                               3,
                                               -1};

  static constexpr PixelFormatDetails kI420 = {PixelFormat::kI420,
                                               ColorSpace::kSYCC,
                                               ChromaSubsampling::kSAMP_420,
                                               3,
                                               kI420Order,
                                               3,
                                               -1};

  static constexpr PixelFormatDetails kI422 = {PixelFormat::kI422,
                                               ColorSpace::kSYCC,
                                               ChromaSubsampling::kSAMP_422,
                                               3,
                                               kI422Order,
                                               3,
                                               -1};

  static constexpr PixelFormatDetails kI440 = {PixelFormat::kI440,
                                               ColorSpace::kSYCC,
                                               ChromaSubsampling::kSAMP_440,
                                               3,
                                               kI440Order,
                                               3,
                                               -1};

  static constexpr PixelFormatDetails kI444 = {PixelFormat::kI444,
                                               ColorSpace::kSYCC,
                                               ChromaSubsampling::kSAMP_444,
                                               3,
                                               kI444Order,
                                               3,
                                               -1};

  // Planar formats
  static constexpr const PixelFormatDetails* kPlanarFormats[] = {
      &kI400,  // Grayscale (Planar)
      &kI411,  // YUV 4:1:1 (Planar)
      &kI420,  // YUV 4:2:0 (Planar)
      &kI422,  // YUV 4:2:2 (Planar)
      &kI440,  // YUV 4:4:0 (Planar)
      &kI444,  // YUV 4:4:4 (Planar)
      &kGRAY,  // Grayscale (just Y component)
      &kYA,    // Grayscale with Alpha
      // Formats that can be planar (supported as planar)
      &kRGB,    // RGB (Interleaved)
      &kRGBA,   // RGBA (Interleaved)
      &kBGRA,   // BGRA (Interleaved)
      &kABGR,   // ABGR (Interleaved)
      &kCMYK,   // CMYK (Interleaved)
      &kCMYKA,  // CMYK with Alpha (Interleaved)
  };

  // Interleaved formats
  static constexpr const PixelFormatDetails* kInterleavedFormats[] = {
      &kUYVY,  // YUV 4:2:2 (Interleaved)
      &kYUY2,  // YUV 4:2:2 (Interleaved)
      &kNV12,  // YUV 4:2:0 (Interleaved)
      &kNV21,  // YUV 4:2:0 (Interleaved)
      // Formats that can be interleaved (supported as interleaved)
      &kRGB,    // RGB (Interleaved)
      &kRGBA,   // RGBA (Interleaved)
      &kBGRA,   // BGRA (Interleaved)
      &kABGR,   // ABGR (Interleaved)
      &kCMYK,   // CMYK (Interleaved)
      &kCMYKA,  // CMYK with Alpha (Interleaved)
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
                                     uint32_t*& comps_width,
                                     uint32_t*& comps_height);
};
