#include "imaging/image_buffer_loader.h"

namespace ws {
namespace imaging {
template <ws::imaging::pixel::IsAllowedPixelNumericType T>
Image* ImageBufferLoader<T>::LoadFromInterleavedBuffer(
    T* buffer, size_t size, uint32_t width, uint32_t height, uint8_t bit_depth,
    const ws::imaging::pixel::PixelFormatDetails* pixel_format_details) {
  uint32_t* comps_width = nullptr;
  uint32_t* comps_height = nullptr;
  uint8_t num_components = pixel_format_details->num_components;
  ChromaSubsampling chroma_subsampling =
      pixel_format_details->chroma_subsampling;
  ColorSpace color_space = pixel_format_details->color_space;
  const uint8_t components_order_size =
      pixel_format_details->components_order_size;
  const uint8_t* components_order_sequence =
      pixel_format_details->components_order_sequence;
  if (!ws::imaging::pixel::PixelFormatConstraints::GetComponentDimensions(
          width, height, num_components, chroma_subsampling,
          pixel_format_details->HasAlpha(), comps_width, comps_height))
    return nullptr;

  size_t total_size = 0;
  int8_t alpha_index = pixel_format_details->alpha_index;
  T* comps_buffer[num_components] = {nullptr};
  size_t comp_size;
  for (size_t i = 0; i < components_order_size; i++) {
    uint8_t c = components_order_sequence[i];
    if (comps_buffer[c]) {
      continue;
    } else {
      comp_size = comps_width[c] * comps_height[c];
    }

    comps_buffer[c] = new T[comp_size];
    total_size += comp_size;
  }

  if (total_size != size) {
    delete[] comps_width;
    delete[] comps_height;
    for (size_t c = 0; c < num_components; c++) {
      delete[] comps_buffer[c];
    }

    return nullptr;
  }

  ImageComponent** components = new ImageComponent*[num_components];
  size_t offset = 0;
  size_t comps_index[num_components];
  for (size_t i = 0; i < num_components; i++) {
    comps_index[i] = 0;
  }

  while (offset < total_size) {
    for (uint8_t i = 0; i < components_order_size; i++) {
      uint8_t c = components_order_sequence[i];
      comps_buffer[c][comps_index[c]++] = buffer[offset];
      offset++;
    }
  }

  for (size_t i = 0; i < num_components; i++) {
    uint8_t c = components_order_sequence[i];
    components[c] =
        new ImageComponent(comps_buffer[c], comps_width[c], comps_height[c],
                           bit_depth, c == alpha_index);
  }

  delete[] comps_width;
  delete[] comps_height;
  return new Image(components, num_components, width, height, color_space,
                   chroma_subsampling);
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
Image* ImageBufferLoader<T>::LoadFromInterleavedBuffer(
    T* buffer, size_t size, uint32_t width, uint32_t height, uint8_t bit_depth,
    const ws::imaging::pixel::PixelFormat pixel_format) {
  if (!buffer || size == 0 || width == 0 || height == 0) return nullptr;
  const ws::imaging::pixel::PixelFormatDetails* pixel_format_details =
      ws::imaging::pixel::PixelFormatConstraints::GetInterleavedFormat(
          pixel_format);
  if (!pixel_format_details) return nullptr;
  return ImageBufferLoader<T>::LoadFromInterleavedBuffer(
      buffer, size, width, height, bit_depth, pixel_format_details);
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
Image* ImageBufferLoader<T>::LoadFromPlanarBuffer(
    T* buffer, size_t size, uint32_t width, uint32_t height, uint8_t bit_depth,
    const ws::imaging::pixel::PixelFormatDetails* pixel_format_details) {
  uint32_t* comps_width = nullptr;
  uint32_t* comps_height = nullptr;
  uint8_t num_components = pixel_format_details->num_components;
  ChromaSubsampling chroma_subsampling =
      pixel_format_details->chroma_subsampling;
  ColorSpace color_space = pixel_format_details->color_space;
  const uint8_t components_order_size =
      pixel_format_details->components_order_size;
  const uint8_t* components_order_sequence =
      pixel_format_details->components_order_sequence;
  if (!ws::imaging::pixel::PixelFormatConstraints::GetComponentDimensions(
          width, height, num_components, chroma_subsampling,
          pixel_format_details->HasAlpha(), comps_width, comps_height))
    return nullptr;
  size_t total_size = 0;
  int8_t alpha_index = pixel_format_details->alpha_index;
  for (uint8_t c = 0; c < num_components; ++c) {
    total_size += comps_width[c] * comps_height[c];
  }

  if (total_size != size) {
    delete[] comps_width;
    delete[] comps_height;
    return nullptr;
  }

  ImageComponent** components = new ImageComponent*[num_components];
  size_t offset = 0;
  uint32_t comp_width, comp_height;
  size_t comp_size;
  for (size_t i = 0; i < components_order_size; i++) {
    uint8_t c = components_order_sequence[i];
    comp_width = comps_width[c];
    comp_height = comps_height[c];
    comp_size = comp_width * comp_height;
    T* comp_buffer = new T[comp_size];
    std::memcpy(comp_buffer, buffer + offset, comp_size * sizeof(T));
    components[c] = new ImageComponent(comp_buffer, comp_width, comp_height,
                                       bit_depth, c == alpha_index);
    offset += comp_size;
  }

  delete[] comps_width;
  delete[] comps_height;
  return new Image(components, num_components, width, height, color_space,
                   chroma_subsampling);
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
Image* ImageBufferLoader<T>::LoadFromPlanarBuffer(
    T* buffer, size_t size, uint32_t width, uint32_t height, uint8_t bit_depth,
    const ws::imaging::pixel::PixelFormat pixel_format) {
  if (!buffer || size == 0 || width == 0 || height == 0) return nullptr;
  const ws::imaging::pixel::PixelFormatDetails* pixel_format_details =
      ws::imaging::pixel::PixelFormatConstraints::GetPlanarFormat(pixel_format);
  if (!pixel_format_details) return nullptr;
  return LoadFromPlanarBuffer(buffer, size, width, height, bit_depth,
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
