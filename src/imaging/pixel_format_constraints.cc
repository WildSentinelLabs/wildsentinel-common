#include "imaging/pixel_format_constraints.h"

const PixelFormatDetails** PixelFormatConstraints::GetPlanarFormats(
    const ColorSpace& color_space, const ChromaSubsampling& chroma_subsampling,
    const uint8_t& num_components, const bool& has_alpha, uint8_t& size) {
  size = 0;
  for (const auto& details : kPlanarFormats) {
    if (details->color_space == color_space &&
        details->chroma_subsampling == chroma_subsampling &&
        details->num_components == num_components &&
        (details->alpha_index != -1) == has_alpha) {
      size++;
    }
  }

  if (size = 0) return nullptr;
  const PixelFormatDetails** supported_formats =
      new const PixelFormatDetails*[size];
  uint8_t index = 0;
  for (const auto& details : kPlanarFormats) {
    if (details->color_space == color_space &&
        details->chroma_subsampling == chroma_subsampling &&
        details->num_components == num_components &&
        (details->alpha_index != -1) == has_alpha) {
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
        (details->alpha_index != -1) == has_alpha) {
      size++;
    }
  }

  if (size = 0) return nullptr;
  const PixelFormatDetails** supported_formats =
      new const PixelFormatDetails*[size];
  uint8_t index = 0;
  for (const auto& details : kInterleavedFormats) {
    if (details->color_space == color_space &&
        details->chroma_subsampling == chroma_subsampling &&
        details->num_components == num_components &&
        (details->alpha_index != -1) == has_alpha) {
      supported_formats[index++] = details;
    }
  }

  return supported_formats;
}
