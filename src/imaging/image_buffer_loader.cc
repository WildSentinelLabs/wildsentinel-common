#include "imaging/image_buffer_loader.h"

namespace ws {
namespace imaging {
template <ws::imaging::pixel::IsAllowedPixelNumericType T>
Image ImageBufferLoader<T>::LoadFromInterleavedBuffer(
    ReadOnlySpan<T> buffer, uint32_t width, uint32_t height, uint8_t bit_depth,
    const ws::imaging::pixel::PixelFormatDetails* pixel_format_details) {
  if (!pixel_format_details ||
      (pixel_format_details->layout &
       ws::imaging::pixel::PixelLayoutFlag::kInterleaved) ==
          static_cast<ws::imaging::pixel::PixelLayoutFlag>(0))
    return Image();

  Array<Point> dimensions =
      ws::imaging::pixel::PixelFormatConstraints::GetDimensions(
          width, height, pixel_format_details->num_components,
          pixel_format_details->chroma_subsampling,
          pixel_format_details->HasAlpha());
  if (dimensions.Empty()) return Image();

  Array<T> comps_buffer[pixel_format_details->num_components];
  for (uint8_t i = 0; i < pixel_format_details->components_order.Length();
       ++i) {
    uint8_t c = pixel_format_details->components_order[i];
    if (comps_buffer[c]) continue;
    comps_buffer[c] = std::move(Array<T>(dimensions[c].x * dimensions[c].y));
  }

  Array<std::unique_ptr<IImageComponent>> components(
      pixel_format_details->num_components);
  size_t offset = 0, comps_index[pixel_format_details->num_components];
  for (size_t i = 0; i < pixel_format_details->num_components; i++) {
    comps_index[i] = 0;
  }

  while (offset < buffer.Length()) {
    for (uint8_t i = 0; i < pixel_format_details->components_order.Length();
         i++) {
      uint8_t c = pixel_format_details->components_order[i];
      comps_buffer[c][comps_index[c]++] = buffer[offset];
      offset++;
    }
  }

  for (size_t i = 0; i < pixel_format_details->num_components; i++) {
    uint8_t c = pixel_format_details->components_order[i];
    components[c] = std::make_unique<ImageComponent<T>>(
        std::move(comps_buffer[c]), dimensions[c].x, dimensions[c].y, bit_depth,
        c == pixel_format_details->alpha_index);
  }

  return Image(std::move(components), width, height,
               pixel_format_details->color_space,
               pixel_format_details->chroma_subsampling);
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
Image ImageBufferLoader<T>::LoadFromInterleavedBuffer(
    ReadOnlySpan<T> buffer, uint32_t width, uint32_t height, uint8_t bit_depth,
    const ws::imaging::pixel::PixelFormat pixel_format) {
  if (buffer.Empty() || width == 0 || height == 0) return Image();

  const ws::imaging::pixel::PixelFormatDetails* pixel_format_details =
      ws::imaging::pixel::PixelFormatConstraints::GetFormat(pixel_format);
  if (!pixel_format_details ||
      (pixel_format_details->layout &
       ws::imaging::pixel::PixelLayoutFlag::kInterleaved) ==
          static_cast<ws::imaging::pixel::PixelLayoutFlag>(0))
    return Image();

  return ImageBufferLoader<T>::LoadFromInterleavedBuffer(
      buffer, width, height, bit_depth, pixel_format_details);
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
Image ImageBufferLoader<T>::LoadFromPlanarBuffer(
    ReadOnlySpan<T> buffer, uint32_t width, uint32_t height, uint8_t bit_depth,
    const ws::imaging::pixel::PixelFormatDetails* pixel_format_details) {
  if (!pixel_format_details ||
      (pixel_format_details->layout &
       ws::imaging::pixel::PixelLayoutFlag::kPlanar) ==
          static_cast<ws::imaging::pixel::PixelLayoutFlag>(0))
    return Image();

  Array<Point> dimensions =
      ws::imaging::pixel::PixelFormatConstraints::GetDimensions(
          width, height, pixel_format_details->num_components,
          pixel_format_details->chroma_subsampling,
          pixel_format_details->HasAlpha());
  if (dimensions.Empty()) return Image();

  Array<std::unique_ptr<IImageComponent>> components(
      pixel_format_details->num_components);
  size_t offset = 0;
  for (size_t i = 0; i < pixel_format_details->components_order.Length(); i++) {
    uint8_t c = pixel_format_details->components_order[i];
    uint32_t comp_width = dimensions[c].x, comp_height = dimensions[c].y;
    size_t comp_size = comp_width * comp_height;
    Array<T> comp_buffer(comp_size);
    std::memcpy(comp_buffer, static_cast<const T*>(buffer) + offset,
                comp_size * sizeof(T));
    components[c] = std::make_unique<ImageComponent<T>>(
        std::move(comp_buffer), comp_width, comp_height, bit_depth,
        c == pixel_format_details->alpha_index);
    offset += comp_size;
  }

  return Image(std::move(components), width, height,
               pixel_format_details->color_space,
               pixel_format_details->chroma_subsampling);
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
Image ImageBufferLoader<T>::LoadFromPlanarBuffer(
    ReadOnlySpan<T> buffer, uint32_t width, uint32_t height, uint8_t bit_depth,
    const ws::imaging::pixel::PixelFormat pixel_format) {
  if (buffer.Empty() || width == 0 || height == 0) return Image();

  const ws::imaging::pixel::PixelFormatDetails* pixel_format_details =
      ws::imaging::pixel::PixelFormatConstraints::GetFormat(pixel_format);
  if (!pixel_format_details ||
      (pixel_format_details->layout &
       ws::imaging::pixel::PixelLayoutFlag::kPlanar) ==
          static_cast<ws::imaging::pixel::PixelLayoutFlag>(0))
    return Image();

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
