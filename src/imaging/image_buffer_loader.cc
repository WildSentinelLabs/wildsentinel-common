#include "imaging/image_buffer_loader.h"

namespace ws {
namespace imaging {
template <ws::imaging::pixel::IsAllowedPixelNumericType T>
std::unique_ptr<Image> ImageBufferLoader<T>::LoadFromInterleavedBuffer(
    const Span<T>& buffer, uint32_t width, uint32_t height, uint8_t bit_depth,
    const ws::imaging::pixel::PixelFormatDetails* pixel_format_details) {
  if (!pixel_format_details ||
      (pixel_format_details->layout &
       ws::imaging::pixel::PixelLayoutFlag::kInterleaved) ==
          static_cast<ws::imaging::pixel::PixelLayoutFlag>(0))
    return nullptr;
  uint8_t num_components = pixel_format_details->num_components;
  ChromaSubsampling chroma_subsampling =
      pixel_format_details->chroma_subsampling;
  ColorSpace color_space = pixel_format_details->color_space;
  ReadOnlySpan<uint8_t> components_order =
      pixel_format_details->components_order;
  Array<std::pair<uint32_t, uint32_t>> dimensions =
      ws::imaging::pixel::PixelFormatConstraints::GetDimensions(
          width, height, num_components, chroma_subsampling,
          pixel_format_details->HasAlpha());
  if (dimensions.IsEmpty()) return nullptr;
  size_t image_size = buffer.Length();
  int8_t alpha_index = pixel_format_details->alpha_index;

  Array<T> comps_buffer[num_components];
  for (uint8_t i = 0; i < components_order.Length(); ++i) {
    uint8_t c = components_order[i];
    if (comps_buffer[c]) continue;
    comps_buffer[c] =
        std::move(Array<T>(dimensions[c].first * dimensions[c].second));
  }

  Array<std::unique_ptr<IImageComponent>> components(num_components);
  size_t offset = 0, comps_index[num_components];
  for (size_t i = 0; i < num_components; i++) {
    comps_index[i] = 0;
  }

  while (offset < image_size) {
    for (uint8_t i = 0; i < components_order.Length(); i++) {
      uint8_t c = components_order[i];
      comps_buffer[c][comps_index[c]++] = buffer[offset];
      offset++;
    }
  }

  for (size_t i = 0; i < num_components; i++) {
    uint8_t c = components_order[i];
    components[c] = std::make_unique<ImageComponent<T>>(
        std::move(comps_buffer[c]), dimensions[c].first, dimensions[c].second,
        bit_depth, c == alpha_index);
  }

  return std::make_unique<Image>(std::move(components), width, height,
                                 color_space, chroma_subsampling);
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
std::unique_ptr<Image> ImageBufferLoader<T>::LoadFromInterleavedBuffer(
    const Span<T>& buffer, uint32_t width, uint32_t height, uint8_t bit_depth,
    const ws::imaging::pixel::PixelFormat pixel_format) {
  if (buffer.IsEmpty() || width == 0 || height == 0) return nullptr;
  const ws::imaging::pixel::PixelFormatDetails* pixel_format_details =
      ws::imaging::pixel::PixelFormatConstraints::GetFormat(pixel_format);
  if (!pixel_format_details ||
      (pixel_format_details->layout &
       ws::imaging::pixel::PixelLayoutFlag::kInterleaved) ==
          static_cast<ws::imaging::pixel::PixelLayoutFlag>(0))
    return nullptr;
  return ImageBufferLoader<T>::LoadFromInterleavedBuffer(
      buffer, width, height, bit_depth, pixel_format_details);
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
std::unique_ptr<Image> ImageBufferLoader<T>::LoadFromPlanarBuffer(
    const Span<T>& buffer, uint32_t width, uint32_t height, uint8_t bit_depth,
    const ws::imaging::pixel::PixelFormatDetails* pixel_format_details) {
  if (!pixel_format_details ||
      (pixel_format_details->layout &
       ws::imaging::pixel::PixelLayoutFlag::kPlanar) ==
          static_cast<ws::imaging::pixel::PixelLayoutFlag>(0))
    return nullptr;
  uint8_t num_components = pixel_format_details->num_components;
  ChromaSubsampling chroma_subsampling =
      pixel_format_details->chroma_subsampling;
  ColorSpace color_space = pixel_format_details->color_space;
  ReadOnlySpan<uint8_t> components_order =
      pixel_format_details->components_order;
  Array<std::pair<uint32_t, uint32_t>> dimensions =
      ws::imaging::pixel::PixelFormatConstraints::GetDimensions(
          width, height, num_components, chroma_subsampling,
          pixel_format_details->HasAlpha());
  if (dimensions.IsEmpty()) return nullptr;
  size_t image_size = buffer.Length();
  int8_t alpha_index = pixel_format_details->alpha_index;

  Array<std::unique_ptr<IImageComponent>> components(num_components);
  size_t offset = 0;
  for (size_t i = 0; i < components_order.Length(); i++) {
    uint8_t c = components_order[i];
    uint32_t comp_width = dimensions[c].first,
             comp_height = dimensions[c].second;
    size_t comp_size = comp_width * comp_height;
    Array<T> comp_buffer(comp_size);
    std::memcpy(comp_buffer, static_cast<const T*>(buffer) + offset,
                comp_size * sizeof(T));
    components[c] = std::make_unique<ImageComponent<T>>(
        std::move(comp_buffer), comp_width, comp_height, bit_depth,
        c == alpha_index);
    offset += comp_size;
  }

  return std::make_unique<Image>(std::move(components), width, height,
                                 color_space, chroma_subsampling);
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
std::unique_ptr<Image> ImageBufferLoader<T>::LoadFromPlanarBuffer(
    const Span<T>& buffer, uint32_t width, uint32_t height, uint8_t bit_depth,
    const ws::imaging::pixel::PixelFormat pixel_format) {
  if (buffer.IsEmpty() || width == 0 || height == 0) return nullptr;
  const ws::imaging::pixel::PixelFormatDetails* pixel_format_details =
      ws::imaging::pixel::PixelFormatConstraints::GetFormat(pixel_format);
  if (!pixel_format_details ||
      (pixel_format_details->layout &
       ws::imaging::pixel::PixelLayoutFlag::kPlanar) ==
          static_cast<ws::imaging::pixel::PixelLayoutFlag>(0))
    return nullptr;
  return LoadFromPlanarBuffer(buffer, width, height, bit_depth,
                              pixel_format_details);
}

template class ImageBufferLoader<uint8_t>;
template class ImageBufferLoader<int8_t>;
template class ImageBufferLoader<uint16_t>;
template class ImageBufferLoader<int16_t>;
template class ImageBufferLoader<uint32_t>;
template class ImageBufferLoader<int32_t>;

}  // namespace imaging
}  // namespace ws
