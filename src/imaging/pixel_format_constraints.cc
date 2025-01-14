#include "imaging/pixel_format_constraints.h"

const PixelFormatDetails** PixelFormatConstraints::GetPlanarFormats(
    const ColorSpace& color_space, const ChromaSubsampling& chroma_subsampling,
    const uint8_t& num_components, const bool& has_alpha, uint8_t& size) {
  size = 0;
  for (const auto& details : kPlanarFormats) {
    if (details->color_space == color_space &&
        details->chroma_subsampling == chroma_subsampling &&
        details->num_components == num_components &&
        details->has_alpha() == has_alpha) {
      size++;
    }
  }

  if (size == 0) return nullptr;
  const PixelFormatDetails** supported_formats =
      new const PixelFormatDetails*[size];
  uint8_t index = 0;
  for (const auto& details : kPlanarFormats) {
    if (details->color_space == color_space &&
        details->chroma_subsampling == chroma_subsampling &&
        details->num_components == num_components &&
        details->has_alpha() == has_alpha) {
      supported_formats[index++] = details;
    }
  }

  return supported_formats;
}

const PixelFormatDetails* PixelFormatConstraints::GetPlanarFormat(
    const PixelFormat& pixel_format) {
  for (const auto& details : kPlanarFormats) {
    if (details->pixel_format == pixel_format) {
      return details;
    }
  }

  return nullptr;
}

const PixelFormatDetails* PixelFormatConstraints::GetInterleavedFormat(
    const PixelFormat& pixel_format) {
  for (const auto& details : kInterleavedFormats) {
    if (details->pixel_format == pixel_format) {
      return details;
    }
  }

  return nullptr;
}

const PixelFormatDetails** PixelFormatConstraints::GetInterleavedFormats(
    const ColorSpace& color_space, const ChromaSubsampling& chroma_subsampling,
    const uint8_t& num_components, const bool& has_alpha, uint8_t& size) {
  size = 0;
  for (const auto& details : kInterleavedFormats) {
    if (details->color_space == color_space &&
        details->chroma_subsampling == chroma_subsampling &&
        details->num_components == num_components &&
        details->has_alpha() == has_alpha) {
      size++;
    }
  }

  if (size == 0) return nullptr;
  const PixelFormatDetails** supported_formats =
      new const PixelFormatDetails*[size];
  uint8_t index = 0;
  for (const auto& details : kInterleavedFormats) {
    if (details->color_space == color_space &&
        details->chroma_subsampling == chroma_subsampling &&
        details->num_components == num_components &&
        details->has_alpha() == has_alpha) {
      supported_formats[index++] = details;
    }
  }

  return supported_formats;
}

bool PixelFormatConstraints::GetComponentDimensions(
    uint32_t width, uint32_t height, uint8_t num_comps,
    ChromaSubsampling chroma_subsampling, uint32_t*& comps_width,
    uint32_t*& comps_height) {
  comps_width = new uint32_t[num_comps];
  comps_height = new uint32_t[num_comps];
  switch (chroma_subsampling) {
    case ChromaSubsampling::kSAMP_444:
      for (uint8_t c = 0; c < num_comps; ++c) {
        comps_width[c] = width;
        comps_height[c] = height;
      }

      return true;
      break;

    case ChromaSubsampling::kSAMP_422:
      if (num_comps != 3) break;
      comps_width[0] = width;
      comps_height[0] = height;
      comps_width[1] = comps_width[2] = width / 2;
      comps_height[1] = comps_height[2] = height;
      return true;
      break;

    case ChromaSubsampling::kSAMP_420:
      if (num_comps != 3) break;
      comps_width[0] = width;
      comps_height[0] = height;
      comps_width[1] = comps_width[2] = width / 2;
      comps_height[1] = comps_height[2] = height / 2;
      return true;
      break;

    case ChromaSubsampling::kSAMP_400:
      if (num_comps != 1) break;
      comps_width[0] = width;
      comps_height[0] = height;
      return true;
      break;

    case ChromaSubsampling::kSAMP_440:
      if (num_comps != 3) break;
      comps_width[0] = width;
      comps_height[0] = height;
      comps_width[1] = comps_width[2] = width;
      comps_height[1] = comps_height[2] = height / 2;
      return true;
      break;

    case ChromaSubsampling::kSAMP_411:
      if (num_comps != 3) break;
      comps_width[0] = width;
      comps_height[0] = height;
      comps_width[1] = comps_width[2] = width / 4;
      comps_height[1] = comps_height[2] = height;
      return true;
      break;

    default:
      delete[] comps_width;
      delete[] comps_height;
      throw std::invalid_argument("Unsupported chroma subsampling.");
  }

  delete[] comps_width;
  delete[] comps_height;
  return false;
}
