#include "imaging/image_buffer_exporter.h"
namespace ws {
namespace imaging {

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
Array<T> ImageBufferExporter<T>::ExportToInterleavedBuffer(
    const Image& image, ws::imaging::pixel::PixelFormat pixel_format) {
  if (!image.IsValid()) return Array<T>();

  const ws::imaging::pixel::PixelFormatDetails* pixel_format_details =
      ws::imaging::pixel::PixelFormatConstraints::GetFormat(pixel_format);
  if (!pixel_format_details ||
      (pixel_format_details->layout &
       ws::imaging::pixel::PixelLayoutFlag::kInterleaved) ==
          static_cast<ws::imaging::pixel::PixelLayoutFlag>(0))
    return Array<T>();

  if (image.GetColorSpace() != pixel_format_details->color_space ||
      image.GetChromaSubsampling() !=
          pixel_format_details->chroma_subsampling ||
      image.NumComponents() != pixel_format_details->num_components ||
      image.HasAlpha() != pixel_format_details->HasAlpha()) {
    return Array<T>();
  }

  const ImageComponent<T>* components[image.NumComponents()] = {nullptr};
  size_t image_size = 0;
  for (uint8_t i = 0; i < pixel_format_details->components_order.Length();
       ++i) {
    uint8_t c = pixel_format_details->components_order[i];
    if (components[c]) continue;
    components[c] =
        dynamic_cast<const ImageComponent<T>*>(image.GetComponent(c));
    image_size += components[c]->Length();
  }

  Array<T> buffer(image_size);
  size_t offset = 0;
  size_t comps_index[image.NumComponents()];
  for (size_t i = 0; i < image.NumComponents(); i++) {
    comps_index[i] = 0;
  }

  while (offset < image_size) {
    for (uint8_t i = 0; i < pixel_format_details->components_order.Length();
         i++) {
      uint8_t c = pixel_format_details->components_order[i];
      buffer[offset] = static_cast<const T*>(*components[c])[comps_index[c]++];
      offset++;
    }
  }

  return buffer;
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
Array<T> ImageBufferExporter<T>::ExportToPlanarBuffer(
    const Image& image, ws::imaging::pixel::PixelFormat pixel_format) {
  if (!image.IsValid()) return Array<T>();
  const ws::imaging::pixel::PixelFormatDetails* pixel_format_details =
      ws::imaging::pixel::PixelFormatConstraints::GetFormat(pixel_format);
  if (!pixel_format_details ||
      (pixel_format_details->layout &
       ws::imaging::pixel::PixelLayoutFlag::kPlanar) ==
          static_cast<ws::imaging::pixel::PixelLayoutFlag>(0))
    return Array<T>();

  if (image.GetColorSpace() != pixel_format_details->color_space ||
      image.GetChromaSubsampling() !=
          pixel_format_details->chroma_subsampling ||
      image.NumComponents() != pixel_format_details->num_components ||
      image.HasAlpha() != pixel_format_details->HasAlpha()) {
    return Array<T>();
  }

  const ImageComponent<T>* components[image.NumComponents()] = {nullptr};
  size_t image_size = 0;
  for (uint8_t i = 0; i < pixel_format_details->components_order.Length();
       ++i) {
    uint8_t c = pixel_format_details->components_order[i];
    if (components[c]) continue;
    components[c] =
        dynamic_cast<const ImageComponent<T>*>(image.GetComponent(c));
    image_size += components[c]->Length();
  }

  Array<T> buffer(image_size);
  size_t offset = 0;
  for (uint8_t i = 0; i < pixel_format_details->components_order.Length();
       ++i) {
    uint8_t c = pixel_format_details->components_order[i];
    std::memcpy(buffer + offset, static_cast<const T*>(*components[c]),
                components[c]->Length() * sizeof(T));
    offset += components[c]->Length();
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
// TODO: Separate common_sequence and non common (use parallelism in common)
