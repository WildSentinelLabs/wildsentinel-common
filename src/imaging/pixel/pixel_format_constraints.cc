#include "imaging/pixel/pixel_format_constraints.h"
namespace ws {
namespace imaging {
namespace pixel {

std::vector<const PixelFormatDetails*> PixelFormatConstraints::formats = {
    &kI411,  // YUV 4:1:1 (Planar)
    &kI420,  // YUV 4:2:0 (Planar)
    &kI422,  // YUV 4:2:2 (Planar)
    &kI440,  // YUV 4:4:0 (Planar)
    &kI441,  // YUV 4:4:0 (Planar)
    &kI444,  // YUV 4:4:4 (Planar)

    &kUyvy,  // YUV 4:2:2 (Interleaved)
    &kYuy2,  // YUV 4:2:2 (Interleaved)
    &kNV12,  // YUV 4:2:0 (Interleaved)
    &kNv21,  // YUV 4:2:0 (Interleaved)

    &kI400,  // Grayscale (Y)
    &kYa,    // Grayscale with Alpha
    &kGray,  // Grayscale

    &kRgb,    // Rgb
    &kRgba,   // Rgba
    &kBgrA,   // BgrA
    &kABgr,   // ABgr
    &kCmyk,   // Cmyk
    &kCmyka,  // Cmyk with Alpha
};

const PixelFormatDetails* PixelFormatConstraints::GetFormat(
    PixelFormat pixel_format) {
  for (const auto& details : formats) {
    if (details->pixel_format == pixel_format) {
      return details;
    }
  }

  return nullptr;
}

Array<Point> PixelFormatConstraints::GetDimensions(
    uint32_t width, uint32_t height, uint8_t num_components,
    ChromaSubsampling chroma_subsampling, bool has_alpha) {
  if (num_components < 1) return Array<Point>::Empty();
  Array<Point> dimensions(num_components);
  uint8_t num_comps = num_components;
  if (has_alpha && num_components > 1) {
    uint8_t alpha_index = num_components - 1;
    dimensions[alpha_index].x = dimensions[alpha_index].y = width;
    num_comps--;
  }

  switch (chroma_subsampling) {
    case ChromaSubsampling::kSamp444:
      for (uint8_t c = 0; c < num_comps; ++c) {
        dimensions[c].x = width;
        dimensions[c].y = height;
      }

      return dimensions;
      break;

    case ChromaSubsampling::kSamp422:
      if (num_comps != 3) break;
      dimensions[0].x = width;
      dimensions[0].y = height;
      dimensions[1].x = dimensions[2].x = width / 2;
      dimensions[1].y = dimensions[2].y = height;
      return dimensions;
      break;

    case ChromaSubsampling::kSamp420:
      if (num_comps != 3) break;
      dimensions[0].x = width;
      dimensions[0].y = height;
      dimensions[1].x = dimensions[2].x = width / 2;
      dimensions[1].y = dimensions[2].y = height / 2;
      return dimensions;
      break;

    case ChromaSubsampling::kSamp400:
      if (num_comps != 1) break;
      for (uint8_t c = 0; c < num_comps; ++c) {
        dimensions[c].x = width;
        dimensions[c].y = height;
      }

      return dimensions;
      break;

    case ChromaSubsampling::kSamp440:
      if (num_comps != 3) break;
      dimensions[0].x = width;
      dimensions[0].y = height;
      dimensions[1].x = dimensions[2].x = width;
      dimensions[1].y = dimensions[2].y = height / 2;
      return dimensions;
      break;

    case ChromaSubsampling::kSamp411:
      if (num_comps != 3) break;
      dimensions[0].x = width;
      dimensions[0].y = height;
      dimensions[1].x = dimensions[2].x = width / 4;
      dimensions[1].y = dimensions[2].y = height;
      return dimensions;
      break;

    case ChromaSubsampling::kSamp441:
      if (num_comps != 3) break;
      dimensions[0].x = width;
      dimensions[0].y = height;
      dimensions[1].x = dimensions[2].x = width;
      dimensions[1].y = dimensions[2].y = height / 4;
      return dimensions;
      break;

    default:
      throw std::invalid_argument("Unsupported chroma subsampling.");
  }

  return Array<Point>::Empty();
}
}  // namespace pixel
}  // namespace imaging
}  // namespace ws
