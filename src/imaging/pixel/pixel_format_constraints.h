#pragma once

#include <vector>

#include "array.h"
#include "imaging/chroma_subsampling.h"
#include "imaging/color_space.h"
#include "imaging/pixel/pixel_allowed_types.h"
#include "imaging/pixel/pixel_format.h"
#include "imaging/pixel/pixel_format_details.h"
#include "imaging/point.h"

namespace ws {
namespace imaging {

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
 private:
  // Pixel format details
  static constexpr PixelFormatDetails kRgb = {
      PixelFormat::kRgb,
      ColorSpace::kSRgb,
      ChromaSubsampling::kSamp444,
      3,
      {kRgbOrder, sizeof(kRgbOrder)},
      -1,
      PixelLayoutFlag::kPlanar | PixelLayoutFlag::kInterleaved,
      true};

  static constexpr PixelFormatDetails kRgba = {
      PixelFormat::kRgba,
      ColorSpace::kSRgb,
      ChromaSubsampling::kSamp444,
      4,
      {kRgbaOrder, sizeof(kRgbaOrder)},
      3,
      PixelLayoutFlag::kPlanar | PixelLayoutFlag::kInterleaved,
      true};

  static constexpr PixelFormatDetails kBgr = {
      PixelFormat::kRgb,
      ColorSpace::kSRgb,
      ChromaSubsampling::kSamp444,
      3,
      {kBgrOrder, sizeof(kBgrOrder)},
      -1,
      PixelLayoutFlag::kPlanar | PixelLayoutFlag::kInterleaved,
      true};

  static constexpr PixelFormatDetails kBgrA = {
      PixelFormat::kBgra,
      ColorSpace::kSRgb,
      ChromaSubsampling::kSamp444,
      4,
      {kBgrAOrder, sizeof(kBgrAOrder)},
      3,
      PixelLayoutFlag::kPlanar | PixelLayoutFlag::kInterleaved,
      true};

  static constexpr PixelFormatDetails kABgr = {
      PixelFormat::kAbgr,
      ColorSpace::kSRgb,
      ChromaSubsampling::kSamp444,
      4,
      {kABgrOrder, sizeof(kABgrOrder)},
      3,
      PixelLayoutFlag::kPlanar | PixelLayoutFlag::kInterleaved,
      true};

  static constexpr PixelFormatDetails kCmyk = {
      PixelFormat::kCmyk,
      ColorSpace::kCmyk,
      ChromaSubsampling::kSamp444,
      4,
      {kCmykOrder, sizeof(kCmykOrder)},
      -1,
      PixelLayoutFlag::kPlanar | PixelLayoutFlag::kInterleaved,
      true};

  static constexpr PixelFormatDetails kCmyka = {
      PixelFormat::kCmyka,
      ColorSpace::kCmyk,
      ChromaSubsampling::kSamp444,
      5,
      {kCmykaOrder, sizeof(kCmykaOrder)},
      4,
      PixelLayoutFlag::kPlanar | PixelLayoutFlag::kInterleaved,
      true};

  static constexpr PixelFormatDetails kGray = {
      PixelFormat::kGray,
      ColorSpace::kGray,
      ChromaSubsampling::kSamp400,
      1,
      {kGrayOrder, sizeof(kGrayOrder)},
      -1,
      PixelLayoutFlag::kPlanar | PixelLayoutFlag::kInterleaved,
      true};

  static constexpr PixelFormatDetails kYa = {
      PixelFormat::kYa,
      ColorSpace::kSYcc,
      ChromaSubsampling::kSamp400,
      2,
      {kYaOrder, sizeof(kYaOrder)},
      1,
      PixelLayoutFlag::kPlanar | PixelLayoutFlag::kInterleaved,
      true};

  static constexpr PixelFormatDetails kUyvy = {PixelFormat::kUyvy,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp422,
                                               3,
                                               {kUyvyOrder, sizeof(kUyvyOrder)},
                                               -1,
                                               PixelLayoutFlag::kInterleaved,
                                               false};

  static constexpr PixelFormatDetails kYuy2 = {PixelFormat::kYuy2,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp422,
                                               3,
                                               {kYuy2Order, sizeof(kYuy2Order)},
                                               -1,
                                               PixelLayoutFlag::kInterleaved,
                                               false};

  static constexpr PixelFormatDetails kNV12 = {PixelFormat::kNv12,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp420,
                                               3,
                                               {kNv12Order, sizeof(kNv12Order)},
                                               -1,
                                               PixelLayoutFlag::kInterleaved,
                                               true};

  static constexpr PixelFormatDetails kNv21 = {PixelFormat::kNv21,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp420,
                                               3,
                                               {kNv21Order, sizeof(kNv21Order)},
                                               -1,
                                               PixelLayoutFlag::kInterleaved,
                                               true};

  static constexpr PixelFormatDetails kI400 = {
      PixelFormat::kI400,
      ColorSpace::kSYcc,
      ChromaSubsampling::kSamp400,
      1,
      {kI400Order, sizeof(kI400Order)},
      -1,
      PixelLayoutFlag::kPlanar | PixelLayoutFlag::kInterleaved,
      true};

  static constexpr PixelFormatDetails kI411 = {PixelFormat::kI411,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp411,
                                               3,
                                               {kI411Order, sizeof(kI411Order)},
                                               -1,
                                               PixelLayoutFlag::kPlanar,
                                               true};

  static constexpr PixelFormatDetails kI420 = {PixelFormat::kI420,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp420,
                                               3,
                                               {kI420Order, sizeof(kI420Order)},
                                               -1,
                                               PixelLayoutFlag::kPlanar,
                                               true};

  static constexpr PixelFormatDetails kI422 = {PixelFormat::kI422,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp422,
                                               3,
                                               {kI422Order, sizeof(kI422Order)},
                                               -1,
                                               PixelLayoutFlag::kPlanar,
                                               true};

  static constexpr PixelFormatDetails kI440 = {PixelFormat::kI440,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp440,
                                               3,
                                               {kI440Order, sizeof(kI440Order)},
                                               -1,
                                               PixelLayoutFlag::kPlanar,
                                               true};

  static constexpr PixelFormatDetails kI441 = {PixelFormat::kI441,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp441,
                                               3,
                                               {kI441Order, sizeof(kI441Order)},
                                               -1,
                                               PixelLayoutFlag::kPlanar,
                                               true};

  static constexpr PixelFormatDetails kI444 = {PixelFormat::kI444,
                                               ColorSpace::kSYcc,
                                               ChromaSubsampling::kSamp444,
                                               3,
                                               {kI444Order, sizeof(kI444Order)},
                                               -1,
                                               PixelLayoutFlag::kPlanar,
                                               true};

 public:
  // Formats
  static std::vector<const PixelFormatDetails*> formats;

  static const PixelFormatDetails* GetFormat(PixelFormat pixel_format);

  static Array<Point> GetDimensions(uint32_t width, uint32_t height,
                                    uint8_t num_components,
                                    ChromaSubsampling chroma_subsampling,
                                    bool has_alpha);
};

// TODO: Enhace pixel_format management and structures if needed

}  // namespace imaging
}  // namespace ws
