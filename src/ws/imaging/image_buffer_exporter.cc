#include "ws/imaging/image_buffer_exporter.h"
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

  Array<Point> dimensions = ws::imaging::PixelFormatConstraints::GetDimensions(
      image.GetComponent(0).Width(), image.GetComponent(0).Height(),
      num_components, pixel_format_details->chroma_subsampling,
      pixel_format_details->HasAlpha());
  if (dimensions.Empty())
    return Status(StatusCode::kBadRequest,
                  "Unsupported dimensions for the given pixel format");

  for (int i = 0; i < num_components; ++i) {
    auto& dim = dimensions[i];
    auto& comp = image.GetComponent(i);
    if (comp.Width() != dim.x || comp.Height() != dim.y) {
      return Status(StatusCode::kBadRequest,
                    "Image component dimensions do not match");
    }
  }

  ws::ReadOnlySpan<uint8_t> components_order =
      pixel_format_details->components_order;
  const size_t num_components_order = components_order.Length();
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

  Array<Point> dimensions = ws::imaging::PixelFormatConstraints::GetDimensions(
      image.GetComponent(0).Width(), image.GetComponent(0).Height(),
      num_components, pixel_format_details->chroma_subsampling,
      pixel_format_details->HasAlpha());
  if (dimensions.Empty())
    return Status(StatusCode::kBadRequest,
                  "Unsupported dimensions for the given pixel format");

  for (int i = 0; i < num_components; ++i) {
    auto& dim = dimensions[i];
    auto& comp = image.GetComponent(i);
    if (comp.Width() != dim.x || comp.Height() != dim.y) {
      return Status(StatusCode::kBadRequest,
                    "Image component dimensions do not match");
    }
  }

  ws::ReadOnlySpan<uint8_t> components_order =
      pixel_format_details->components_order;
  const size_t num_components_order = components_order.Length();
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

template class ImageBufferExporter<uint8_t>;
template class ImageBufferExporter<int8_t>;
template class ImageBufferExporter<uint16_t>;
template class ImageBufferExporter<int16_t>;
template class ImageBufferExporter<uint32_t>;
template class ImageBufferExporter<int32_t>;
}  // namespace imaging
}  // namespace ws
