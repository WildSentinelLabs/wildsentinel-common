#pragma once
#include "imaging/image_defs.h"

struct PixelFormatDetails {
  PixelFormat pixel_format;
  ColorSpace color_space;
  ChromaSubsampling chroma_subsampling;
  uint8_t num_components;
  const uint8_t* component_order_values;
  uint8_t components_order_size;
};

struct PixelFormatConstraints {
  // Component orders
  static constexpr uint8_t kRGBOrder[3] = {0, 1, 2};
  static constexpr uint8_t kRGBAOrder[4] = {0, 1, 2, 3};
  static constexpr uint8_t kBGROrder[3] = {2, 1, 0};
  static constexpr uint8_t kBGRAOrder[4] = {2, 1, 0, 3};
  static constexpr uint8_t kABGROrder[4] = {3, 2, 1, 0};
  static constexpr uint8_t kCMYKOrder[4] = {0, 1, 2, 3};
  static constexpr uint8_t kCMYKAlphaOrder[5] = {0, 1, 2, 3, 4};
  static constexpr uint8_t kYUVOrder[3] = {0, 1, 2};
  static constexpr uint8_t kGrayscaleOrder[1] = {0};
  static constexpr uint8_t kYAOrder[2] = {0, 1};
  static constexpr uint8_t kUYVYIOrder[4] = {1, 0, 0, 2};
  static constexpr uint8_t kYUY2IOrder[4] = {0, 1, 0, 2};
  static constexpr uint8_t kNV12IOrder[3] = {0, 1, 2};
  static constexpr uint8_t kNV21IOrder[3] = {0, 2, 1};
  static constexpr uint8_t kI420POrder[3] = {0, 1, 2};
  static constexpr uint8_t kI422POrder[3] = {0, 1, 2};
  static constexpr uint8_t kI444POrder[3] = {0, 1, 2};

  // Pixel format details
  static constexpr PixelFormatDetails kRGB = {PixelFormat::kRGB,
                                              ColorSpace::kRGB,
                                              3,
                                              ChromaSubsampling::kSAMP_444,
                                              kRGBOrder,
                                              3};
  static constexpr PixelFormatDetails kRGBA = {
      PixelFormat::kRGBA,           ColorSpace::kRGB, 4,
      ChromaSubsampling::kSAMP_444, kRGBAOrder,       4};
  static constexpr PixelFormatDetails kBGR = {PixelFormat::kBGR,
                                              ColorSpace::kRGB,
                                              3,
                                              ChromaSubsampling::kSAMP_444,
                                              kBGROrder,
                                              3};
  static constexpr PixelFormatDetails kBGRA = {
      PixelFormat::kBGRA,           ColorSpace::kRGB, 4,
      ChromaSubsampling::kSAMP_444, kBGRAOrder,       4};
  static constexpr PixelFormatDetails kABGR = {
      PixelFormat::kABGR,           ColorSpace::kRGB, 4,
      ChromaSubsampling::kSAMP_444, kABGROrder,       4};
  static constexpr PixelFormatDetails kCMYK = {
      PixelFormat::kCMYK,           ColorSpace::kCMYK, 4,
      ChromaSubsampling::kSAMP_444, kCMYKOrder,        4};
  static constexpr PixelFormatDetails kCMYKAlpha = {
      PixelFormat::kCMYK_Alpha,     ColorSpace::kCMYK, 5,
      ChromaSubsampling::kSAMP_444, kCMYKAlphaOrder,   5};
  static constexpr PixelFormatDetails kYUV444 = {
      PixelFormat::kYUV444,         ColorSpace::kYUV, 3,
      ChromaSubsampling::kSAMP_444, kYUVOrder,        3};
  static constexpr PixelFormatDetails kYUV422 = {
      PixelFormat::kYUV422,         ColorSpace::kYUV, 3,
      ChromaSubsampling::kSAMP_422, kYUVOrder,        3};
  static constexpr PixelFormatDetails kYUV420 = {
      PixelFormat::kYUV420,         ColorSpace::kYUV, 3,
      ChromaSubsampling::kSAMP_420, kYUVOrder,        3};
  static constexpr PixelFormatDetails kYUV411 = {
      PixelFormat::kYUV411,         ColorSpace::kYUV, 3,
      ChromaSubsampling::kSAMP_411, kYUVOrder,        3};
  static constexpr PixelFormatDetails kYUV440 = {
      PixelFormat::kYUV440,         ColorSpace::kYUV, 3,
      ChromaSubsampling::kSAMP_440, kYUVOrder,        3};
  static constexpr PixelFormatDetails kYUV400 = {
      PixelFormat::kYUV400,         ColorSpace::kGRAYSCALE, 1,
      ChromaSubsampling::kSAMP_400, kGrayscaleOrder,        1};
  static constexpr PixelFormatDetails kYA = {PixelFormat::kYA,
                                             ColorSpace::kGRAYSCALE,
                                             2,
                                             ChromaSubsampling::kSAMP_400,
                                             kYAOrder,
                                             2};
  static constexpr PixelFormatDetails kUYVYI = {
      PixelFormat::kUYVYI,          ColorSpace::kYUV, 3,
      ChromaSubsampling::kSAMP_422, kUYVYIOrder,      4};
  static constexpr PixelFormatDetails kYUY2I = {
      PixelFormat::kYUY2I,          ColorSpace::kYUV, 3,
      ChromaSubsampling::kSAMP_422, kYUY2IOrder,      4};
  static constexpr PixelFormatDetails kNV12I = {
      PixelFormat::kNV12I,          ColorSpace::kYUV, 3,
      ChromaSubsampling::kSAMP_420, kNV12IOrder,      3};
  static constexpr PixelFormatDetails kNV21I = {
      PixelFormat::kNV21I,          ColorSpace::kYUV, 3,
      ChromaSubsampling::kSAMP_420, kNV21IOrder,      3};
  static constexpr PixelFormatDetails kI420P = {
      PixelFormat::kI420P,          ColorSpace::kYUV, 3,
      ChromaSubsampling::kSAMP_420, kI420POrder,      3};
  static constexpr PixelFormatDetails kI422P = {
      PixelFormat::kI422P,          ColorSpace::kYUV, 3,
      ChromaSubsampling::kSAMP_422, kI422POrder,      3};
  static constexpr PixelFormatDetails kI444P = {
      PixelFormat::kI444P,          ColorSpace::kYUV, 3,
      ChromaSubsampling::kSAMP_444, kI444POrder,      3};
};
