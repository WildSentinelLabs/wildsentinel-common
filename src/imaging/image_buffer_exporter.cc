#include "imaging/image_buffer_exporter.h"
namespace ws {
namespace imaging {

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
Array<T> ImageBufferExporter<T>::ExportToInterleavedBuffer(
    const Image& image, ws::imaging::pixel::PixelFormat pixel_format) {
  size_t image_size = 0;
  if (!image.IsValid()) throw std::invalid_argument("Invalid image");
  for (auto& comp : image.Components()) {
    if (comp.GetBufferType() != ImageBufferTypeOf<T>::value)
      throw std::invalid_argument("Buffer type mismatch");

    image_size += comp.Length();
  }

  const ws::imaging::pixel::PixelFormatDetails* pixel_format_details =
      ws::imaging::pixel::PixelFormatConstraints::GetFormat(pixel_format);
  if (!pixel_format_details)
    throw std::invalid_argument(
        "Unsupported pixel format " +
        ws::imaging::pixel::PixelFormatToString(pixel_format));

  if ((pixel_format_details->layout &
       ws::imaging::pixel::PixelLayoutFlag::kInterleaved) ==
      static_cast<ws::imaging::pixel::PixelLayoutFlag>(0))
    throw std::invalid_argument(
        "Pixel format details must indicate interleaved layout");

  if (image.GetColorSpace() != pixel_format_details->color_space ||
      image.GetChromaSubsampling() !=
          pixel_format_details->chroma_subsampling ||
      image.NumComponents() != pixel_format_details->num_components ||
      image.HasAlpha() != pixel_format_details->HasAlpha()) {
    throw std::invalid_argument(
        "Image properties do not match pixel format details");
  }

  ws::ReadOnlySpan<uint8_t> components_order =
      pixel_format_details->components_order;
  if (image_size % components_order.Length() != 0)
    throw std::invalid_argument(
        "Image Size must be divisible by components order length");

  size_t comps_index[pixel_format_details->num_components] = {0};
  T* comps_buffer_in_order[components_order.Length()];
  size_t* comps_buffer_index_in_order[components_order.Length()];
  for (uint8_t i = 0; i < components_order.Length(); ++i) {
    uint8_t c = components_order[i];
    comps_buffer_in_order[i] = image.GetComponent(c).Buffer<T>();
    comps_buffer_index_in_order[i] = &comps_index[c];
  }

  Array<T> buffer(image_size);
  for (size_t offset = 0; offset < buffer.Length();
       offset += components_order.Length()) {
    for (uint8_t i = 0; i < components_order.Length(); ++i) {
      buffer[offset + i] =
          comps_buffer_in_order[i][*comps_buffer_index_in_order[i]++];
    }
  }

  return buffer;
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
Array<T> ImageBufferExporter<T>::ExportToPlanarBuffer(
    const Image& image, ws::imaging::pixel::PixelFormat pixel_format) {
  size_t image_size = 0;
  if (!image.IsValid()) throw std::invalid_argument("Invalid image");
  for (auto& comp : image.Components()) {
    if (comp.GetBufferType() != ImageBufferTypeOf<T>::value)
      throw std::invalid_argument("Buffer type mismatch");

    image_size += comp.Length();
  }

  const ws::imaging::pixel::PixelFormatDetails* pixel_format_details =
      ws::imaging::pixel::PixelFormatConstraints::GetFormat(pixel_format);
  if (!pixel_format_details)
    throw std::invalid_argument(
        "Unsupported pixel format " +
        ws::imaging::pixel::PixelFormatToString(pixel_format));

  if ((pixel_format_details->layout &
       ws::imaging::pixel::PixelLayoutFlag::kPlanar) ==
      static_cast<ws::imaging::pixel::PixelLayoutFlag>(0))
    throw std::invalid_argument(
        "Pixel format details must indicate planar layout");

  if (image.GetColorSpace() != pixel_format_details->color_space ||
      image.GetChromaSubsampling() !=
          pixel_format_details->chroma_subsampling ||
      image.NumComponents() != pixel_format_details->num_components ||
      image.HasAlpha() != pixel_format_details->HasAlpha()) {
    throw std::invalid_argument(
        "Image properties do not match pixel format details");
  }

  ws::ReadOnlySpan<uint8_t> components_order =
      pixel_format_details->components_order;
  if (image_size % components_order.Length() != 0)
    throw std::invalid_argument(
        "Image Size must be divisible by components order length");

  Array<T> buffer(image_size);
  T* buffer_ptr = static_cast<T*>(buffer);
  for (uint8_t i = 0; i < components_order.Length(); ++i) {
    uint8_t c = components_order[i];
    std::memcpy(buffer_ptr, image.GetComponent(c).Buffer<T>(),
                image.GetComponent(c).Length() * sizeof(T));
    buffer_ptr += image.GetComponent(c).Length();
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
