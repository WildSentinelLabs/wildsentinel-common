#include "imaging/pixel_formats/pixel_format_constraints.h"

const PixelFormatDetails** PixelFormatConstraints::GetPlanarFormats(
    const ColorSpace& color_space, const ChromaSubsampling& chroma_subsampling,
    const uint8_t& num_components, const bool& has_alpha, uint8_t& size) {
  size = 0;
  for (const auto& details : kPlanarFormats) {
    if (details->color_space == color_space &&
        details->chroma_subsampling == chroma_subsampling &&
        details->num_components == num_components &&
        details->HasAlpha() == has_alpha) {
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
        details->HasAlpha() == has_alpha) {
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
        details->HasAlpha() == has_alpha) {
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
        details->HasAlpha() == has_alpha) {
      supported_formats[index++] = details;
    }
  }

  return supported_formats;
}

bool PixelFormatConstraints::GetComponentDimensions(
    uint32_t width, uint32_t height, uint8_t num_components,
    ChromaSubsampling chroma_subsampling, bool has_alpha,
    uint32_t*& comps_width, uint32_t*& comps_height) {
  comps_width = new uint32_t[num_components];
  comps_height = new uint32_t[num_components];
  switch (chroma_subsampling) {
    case ChromaSubsampling::kSamp444:
      for (uint8_t c = 0; c < num_components; ++c) {
        comps_width[c] = width;
        comps_height[c] = height;
      }

      return true;
      break;

    case ChromaSubsampling::kSamp422:
      if (num_components != 3) break;
      comps_width[0] = width;
      comps_height[0] = height;
      comps_width[1] = comps_width[2] = width / 2;
      comps_height[1] = comps_height[2] = height;
      return true;
      break;

    case ChromaSubsampling::kSamp420:
      if (num_components != 3) break;
      comps_width[0] = width;
      comps_height[0] = height;
      comps_width[1] = comps_width[2] = width / 2;
      comps_height[1] = comps_height[2] = height / 2;
      return true;
      break;

    case ChromaSubsampling::kSamp400:
      if (has_alpha && num_components != 2)
        break;
      else if (num_components != 1)
        break;
      for (uint8_t c = 0; c < num_components; ++c) {
        comps_width[c] = width;
        comps_height[c] = height;
      }

      return true;
      break;

    case ChromaSubsampling::kSamp440:
      if (num_components != 3) break;
      comps_width[0] = width;
      comps_height[0] = height;
      comps_width[1] = comps_width[2] = width;
      comps_height[1] = comps_height[2] = height / 2;
      return true;
      break;

    case ChromaSubsampling::kSamp411:
      if (num_components != 3) break;
      comps_width[0] = width;
      comps_height[0] = height;
      comps_width[1] = comps_width[2] = width / 4;
      comps_height[1] = comps_height[2] = height;
      return true;
      break;

    case ChromaSubsampling::kSamp441:
      if (num_components != 3) break;
      comps_width[0] = width;
      comps_height[0] = height;
      comps_width[1] = comps_width[2] = width;
      comps_height[1] = comps_height[2] = height / 4;
      return true;
      break;

    default:
      delete[] comps_width;
      delete[] comps_height;
      comps_width = nullptr;
      comps_height = nullptr;
      throw std::invalid_argument("Unsupported chroma subsampling.");
  }

  delete[] comps_width;
  delete[] comps_height;
  comps_width = nullptr;
  comps_height = nullptr;
  return false;
}
