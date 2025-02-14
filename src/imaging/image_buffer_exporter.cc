#include "imaging/image_buffer_exporter.h"
namespace ws {
namespace imaging {

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
T* ImageBufferExporter<T>::ExportToInterleavedBuffer(
    const Image& image, const ws::imaging::pixel::PixelFormat& pixel_format) {
  if (!image.IsValid()) return nullptr;
  const ws::imaging::pixel::PixelFormatDetails* pixel_format_details =
      ws::imaging::pixel::PixelFormatConstraints::GetInterleavedFormat(
          pixel_format);
  if (!pixel_format_details) return nullptr;
  if (image.GetColorSpace() != pixel_format_details->color_space ||
      image.GetChromaSubsampling() !=
          pixel_format_details->chroma_subsampling ||
      image.GetNumComponents() != pixel_format_details->num_components ||
      image.HasAlpha() != pixel_format_details->HasAlpha()) {
    return nullptr;
  }

  const uint8_t components_order_size =
      pixel_format_details->components_order_size;
  const uint8_t* components_order_sequence =
      pixel_format_details->components_order_sequence;
  const ImageComponent* comp = nullptr;
  T* comps_buffer[image.GetNumComponents()] = {nullptr};
  size_t total_size = 0;
  for (uint8_t i = 0; i < components_order_size; ++i) {
    uint8_t c = components_order_sequence[i];
    if (comps_buffer[c]) {
      continue;
    } else {
      comp = image.GetComponent(c);
    }

    comps_buffer[c] = (T*)comp->GetBuffer();
    total_size += comp->GetSize();
  }

  T* buffer = new T[total_size];
  size_t offset = 0;
  size_t comps_index[image.GetNumComponents()];
  for (size_t i = 0; i < image.GetNumComponents(); i++) {
    comps_index[i] = 0;
  }

  while (offset < total_size) {
    for (uint8_t i = 0; i < components_order_size; i++) {
      uint8_t c = components_order_sequence[i];
      buffer[offset] = comps_buffer[c][comps_index[c]++];
      offset++;
    }
  }

  return buffer;
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
T* ImageBufferExporter<T>::ExportToPlanarBuffer(
    const Image& image, const ws::imaging::pixel::PixelFormat& pixel_format) {
  if (!image.IsValid()) return nullptr;
  const ws::imaging::pixel::PixelFormatDetails* pixel_format_details =
      ws::imaging::pixel::PixelFormatConstraints::GetPlanarFormat(pixel_format);
  if (!pixel_format_details) return nullptr;
  if (image.GetColorSpace() != pixel_format_details->color_space ||
      image.GetChromaSubsampling() !=
          pixel_format_details->chroma_subsampling ||
      image.GetNumComponents() != pixel_format_details->num_components ||
      image.HasAlpha() != pixel_format_details->HasAlpha()) {
    return nullptr;
  }

  const uint8_t components_order_size =
      pixel_format_details->components_order_size;
  const uint8_t* components_order_sequence =
      pixel_format_details->components_order_sequence;
  const ImageComponent* comp = nullptr;
  size_t comps_size[image.GetNumComponents()];
  T* comps_buffer[image.GetNumComponents()] = {nullptr};
  size_t total_size = 0;
  for (uint8_t i = 0; i < components_order_size; ++i) {
    uint8_t c = components_order_sequence[i];
    if (comps_buffer[c]) {
      continue;
    } else {
      comp = image.GetComponent(c);
    }

    comps_buffer[c] = (T*)comp->GetBuffer();
    comps_size[c] = comp->GetSize();
    total_size += comps_size[c];
  }

  T* buffer = new T[total_size];
  size_t offset = 0;
  for (uint8_t i = 0; i < components_order_size; ++i) {
    uint8_t c = components_order_sequence[i];
    std::memcpy(buffer + offset, comps_buffer[c], comps_size[c] * sizeof(T));
    offset += comps_size[c];
  }

  return buffer;
}

template class ImageBufferExporter<uint8_t>;
template class ImageBufferExporter<int8_t>;
template class ImageBufferExporter<uint16_t>;
template class ImageBufferExporter<int16_t>;
template class ImageBufferExporter<uint32_t>;
template class ImageBufferExporter<int32_t>;
}  // namespace imaging
}  // namespace ws
