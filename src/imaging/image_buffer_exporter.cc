#include "imaging/image_buffer_exporter.h"
namespace ws {
namespace imaging {

template <ws::imaging::IsAllowedPixelNumericType T>
StatusOr<Array<T>> ImageBufferExporter<T>::ExportToInterleavedBuffer(
    const Image& image, ws::imaging::PixelFormat pixel_format) {
  size_t image_size = 0;
  if (!image.IsValid()) return Status(StatusCode::kBadRequest, "Invalid image");
  const Array<ImageComponent>& components = image.Components();
  for (const auto& comp : components) {
    if (comp.GetBufferType() != ImageBufferTypeOf<T>::value)
      return Status(StatusCode::kBadRequest, "Buffer type mismatch");

    image_size += comp.Length();
  }

  const ws::imaging::PixelFormatDetails* pixel_format_details =
      ws::imaging::PixelFormatConstraints::GetFormat(pixel_format);
  if (!pixel_format_details)
    return Status(StatusCode::kBadRequest,
                  "Unsupported pixel format " +
                      ws::imaging::PixelFormatToString(pixel_format));

  if ((pixel_format_details->layout &
       ws::imaging::PixelLayoutFlag::kInterleaved) ==
      static_cast<ws::imaging::PixelLayoutFlag>(0))
    return Status(StatusCode::kBadRequest,
                  "Pixel format details must indicate interleaved layout");

  const uint8_t num_components = pixel_format_details->num_components;
  if (image.GetColorSpace() != pixel_format_details->color_space ||
      image.GetChromaSubsampling() !=
          pixel_format_details->chroma_subsampling ||
      image.NumComponents() != num_components ||
      image.HasAlpha() != pixel_format_details->HasAlpha()) {
    return Status(StatusCode::kBadRequest,
                  "Image properties do not match pixel format details");
  }

  ws::ReadOnlySpan<uint8_t> components_order =
      pixel_format_details->components_order;
  const size_t num_components_order = components_order.Length();
  if (image_size % num_components_order != 0)
    return Status(StatusCode::kBadRequest,
                  "Image Size must be divisible by components order length");

  size_t comps_index[num_components] = {0};
  T* comps_buffer_in_order[num_components_order];
  size_t* comps_buffer_index_in_order[num_components_order];
  for (size_t i = 0; i < num_components_order; ++i) {
    uint8_t c = components_order[i];
    comps_buffer_in_order[i] = components[c].Buffer<T>();
    comps_buffer_index_in_order[i] = &comps_index[c];
  }

  Array<T> buffer(image_size);
  if (pixel_format_details->has_common_order) {
    const size_t num_pixels = image_size / num_components;
#pragma omp parallel for
    for (size_t i = 0; i < num_pixels; ++i) {
      for (size_t c = 0; c < num_components_order; ++c) {
        buffer[i * num_components_order + c] = comps_buffer_in_order[c][i];
      }
    }
  } else {
    for (size_t offset = 0; offset < buffer.Length();
         offset += num_components_order) {
      for (size_t i = 0; i < num_components_order; ++i) {
        buffer[offset + i] =
            comps_buffer_in_order[i][(*comps_buffer_index_in_order[i])++];
      }
    }
  }

  return buffer;
}

template <ws::imaging::IsAllowedPixelNumericType T>
StatusOr<Array<T>> ImageBufferExporter<T>::ExportToPlanarBuffer(
    const Image& image, ws::imaging::PixelFormat pixel_format) {
  size_t image_size = 0;
  if (!image.IsValid()) return Status(StatusCode::kBadRequest, "Invalid image");
  const Array<ImageComponent>& components = image.Components();
  for (const auto& comp : components) {
    if (comp.GetBufferType() != ImageBufferTypeOf<T>::value)
      return Status(StatusCode::kBadRequest, "Buffer type mismatch");

    image_size += comp.Length();
  }

  const ws::imaging::PixelFormatDetails* pixel_format_details =
      ws::imaging::PixelFormatConstraints::GetFormat(pixel_format);
  if (!pixel_format_details)
    return Status(StatusCode::kBadRequest,
                  "Unsupported pixel format " +
                      ws::imaging::PixelFormatToString(pixel_format));

  if ((pixel_format_details->layout & ws::imaging::PixelLayoutFlag::kPlanar) ==
      static_cast<ws::imaging::PixelLayoutFlag>(0))
    return Status(StatusCode::kBadRequest,
                  "Pixel format details must indicate planar layout");

  const uint8_t num_components = pixel_format_details->num_components;
  if (image.GetColorSpace() != pixel_format_details->color_space ||
      image.GetChromaSubsampling() !=
          pixel_format_details->chroma_subsampling ||
      image.NumComponents() != num_components ||
      image.HasAlpha() != pixel_format_details->HasAlpha()) {
    return Status(StatusCode::kBadRequest,
                  "Image properties do not match pixel format details");
  }

  ws::ReadOnlySpan<uint8_t> components_order =
      pixel_format_details->components_order;
  const size_t num_components_order = components_order.Length();
  if (image_size % num_components_order != 0)
    return Status(StatusCode::kBadRequest,
                  "Image Size must be divisible by components order length");

  Array<T> buffer(image_size);
  T* buffer_ptr = static_cast<T*>(buffer);
  for (size_t i = 0; i < num_components_order; ++i) {
    uint8_t c = components_order[i];
    std::memcpy(buffer_ptr, components[c].Buffer<T>(),
                components[c].Length() * sizeof(T));
    buffer_ptr += components[c].Length();
  }

  return buffer;
}

template struct ImageBufferExporter<uint8_t>;
template struct ImageBufferExporter<int8_t>;
template struct ImageBufferExporter<uint16_t>;
template struct ImageBufferExporter<int16_t>;
template struct ImageBufferExporter<uint32_t>;
template struct ImageBufferExporter<int32_t>;
}  // namespace imaging
}  // namespace ws
