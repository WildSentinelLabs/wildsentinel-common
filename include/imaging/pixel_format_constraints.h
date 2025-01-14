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
static constexpr uint8_t kUYVYIOrder[4] = {1, 0, 0, 2};  // Y, U, Y, V
static constexpr uint8_t kYUY2IOrder[4] = {0, 1, 0, 2};  // Y, U, Y, V
static constexpr uint8_t kNV12IOrder[3] = {0, 1, 2};     // Y, Cb, Cr
static constexpr uint8_t kNV21IOrder[3] = {0, 2, 1};     // Y, Cr, Cb
static constexpr uint8_t kI420POrder[3] = {0, 1, 2};     // Y, Cb, Cr (Planar)
static constexpr uint8_t kI422POrder[3] = {0, 1, 2};     // Y, Cb, Cr (Planar)
static constexpr uint8_t kI444POrder[3] = {0, 1, 2};     // Y, Cb, Cr (Planar)
static constexpr uint8_t kI400POrder[1] = {0};           // Y, Cb, Cr (planar)
static constexpr uint8_t kI411POrder[3] = {
    0, 1, 2};  // Y, Cb, Cr (Planar) with 4:1:1 sub-sampling
static constexpr uint8_t kI440POrder[3] = {
    0, 1, 2};  // Y, Cb, Cr (Planar) with 4:4:0 sub-sampling

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

  static constexpr PixelFormatDetails kUYVYI = {PixelFormat::kUYVYI,
                                                ColorSpace::kSYCC,
                                                ChromaSubsampling::kSAMP_422,
                                                3,
                                                kUYVYIOrder,
                                                4,
                                                -1};

  static constexpr PixelFormatDetails kYUY2I = {PixelFormat::kYUY2I,
                                                ColorSpace::kSYCC,
                                                ChromaSubsampling::kSAMP_422,
                                                3,
                                                kYUY2IOrder,
                                                4,
                                                -1};

  static constexpr PixelFormatDetails kNV12I = {PixelFormat::kNV12I,
                                                ColorSpace::kSYCC,
                                                ChromaSubsampling::kSAMP_420,
                                                3,
                                                kNV12IOrder,
                                                4,
                                                -1};

  static constexpr PixelFormatDetails kNV21I = {PixelFormat::kNV21I,
                                                ColorSpace::kSYCC,
                                                ChromaSubsampling::kSAMP_420,
                                                3,
                                                kNV21IOrder,
                                                4,
                                                -1};

  static constexpr PixelFormatDetails kI400P = {PixelFormat::kI400P,
                                                ColorSpace::kGRAY,
                                                ChromaSubsampling::kSAMP_400,
                                                1,
                                                kI400POrder,
                                                1,
                                                -1};

  static constexpr PixelFormatDetails kI411P = {PixelFormat::kI411P,
                                                ColorSpace::kSYCC,
                                                ChromaSubsampling::kSAMP_411,
                                                3,
                                                kI411POrder,
                                                3,
                                                -1};

  static constexpr PixelFormatDetails kI420P = {PixelFormat::kI420P,
                                                ColorSpace::kSYCC,
                                                ChromaSubsampling::kSAMP_420,
                                                3,
                                                kI420POrder,
                                                3,
                                                -1};

  static constexpr PixelFormatDetails kI422P = {PixelFormat::kI422P,
                                                ColorSpace::kSYCC,
                                                ChromaSubsampling::kSAMP_422,
                                                3,
                                                kI422POrder,
                                                3,
                                                -1};

  static constexpr PixelFormatDetails kI440P = {PixelFormat::kI440P,
                                                ColorSpace::kSYCC,
                                                ChromaSubsampling::kSAMP_440,
                                                3,
                                                kI440POrder,
                                                3,
                                                -1};

  static constexpr PixelFormatDetails kI444P = {PixelFormat::kI444P,
                                                ColorSpace::kSYCC,
                                                ChromaSubsampling::kSAMP_444,
                                                3,
                                                kI444POrder,
                                                3,
                                                -1};

  // Planar formats
  static constexpr const PixelFormatDetails* kPlanarFormats[] = {
      &kI400P,  // Grayscale (Planar)
      &kI411P,  // YUV 4:1:1 (Planar)
      &kI420P,  // YUV 4:2:0 (Planar)
      &kI422P,  // YUV 4:2:2 (Planar)
      &kI440P,  // YUV 4:4:0 (Planar)
      &kI444P,  // YUV 4:4:4 (Planar)
      &kGRAY,   // Grayscale (just Y component)
      &kYA,     // Grayscale with Alpha
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
      &kUYVYI,  // YUV 4:2:2 (Interleaved)
      &kYUY2I,  // YUV 4:2:2 (Interleaved)
      &kNV12I,  // YUV 4:2:0 (Interleaved)
      &kNV21I,  // YUV 4:2:0 (Interleaved)
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
};
