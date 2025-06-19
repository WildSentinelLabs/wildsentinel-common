#include "imaging/image_buffer_loader.h"

namespace ws {
namespace imaging {
template <ws::imaging::pixel::IsAllowedPixelNumericType T>
StatusOr<Image> ImageBufferLoader<T>::LoadFromInterleavedBuffer(
    ReadOnlySpan<T> buffer, uint32_t width, uint32_t height, uint8_t bit_depth,
    const ws::imaging::pixel::PixelFormatDetails* pixel_format_details) {
  if (width == 0)
    return Status(StatusCode::kBadRequest, "Width must be greater than 0");
  if (height == 0)
    return Status(StatusCode::kBadRequest, "Height must be greater than 0");
  if (buffer.Empty())
    return Status(StatusCode::kBadRequest, "Buffer must not be empty");
  if (DetermineImageBufferType(bit_depth) != ImageBufferTypeOf<T>::value)
    return Status(StatusCode::kBadRequest,
                  "Bit depth does not match buffer type");

  if (!pixel_format_details)
    return Status(StatusCode::kBadRequest,
                  "Pixel format details must not be null");

  if ((pixel_format_details->layout &
       ws::imaging::pixel::PixelLayoutFlag::kInterleaved) ==
      static_cast<ws::imaging::pixel::PixelLayoutFlag>(0))
    return Status(StatusCode::kBadRequest,
                  "Pixel format details must indicate interleaved layout");

  Array<Point> dimensions =
      ws::imaging::pixel::PixelFormatConstraints::GetDimensions(
          width, height, pixel_format_details->num_components,
          pixel_format_details->chroma_subsampling,
          pixel_format_details->HasAlpha());
  if (dimensions.Empty())
    return Status(StatusCode::kBadRequest,
                  "Unsupported dimensions for the given pixel format");

  ws::ReadOnlySpan<uint8_t> components_order =
      pixel_format_details->components_order;
  if (buffer.Length() % components_order.Length() != 0)
    return Status(StatusCode::kBadRequest,
                  "Buffer length must be divisible by components order length");

  size_t comps_index[pixel_format_details->num_components] = {0};
  Array<ImageComponent> components(pixel_format_details->num_components);
  T* comps_buffer_in_order[components_order.Length()];
  size_t* comps_buffer_index_in_order[components_order.Length()];
  for (uint8_t i = 0; i < components_order.Length(); ++i) {
    uint8_t c = components_order[i];
    if (components[c].Empty()) {
      ASSIGN_OR_RETURN(components[c],
                       std::move(ImageComponent::Create<T>(
                           static_cast<uint32_t>(dimensions[c].x),
                           dimensions[c].x * dimensions[c].y, bit_depth,
                           c == pixel_format_details->alpha_index)));
    }

    comps_buffer_in_order[i] = components[c].Buffer<T>();
    comps_buffer_index_in_order[i] = &comps_index[c];
  }

  for (size_t offset = 0; offset < buffer.Length();
       offset += components_order.Length()) {
    for (uint8_t i = 0; i < components_order.Length(); ++i) {
      comps_buffer_in_order[i][*comps_buffer_index_in_order[i]++] =
          buffer[offset + i];
    }
  }

  return Image::Create(std::move(components), width, height,
                       pixel_format_details->color_space,
                       pixel_format_details->chroma_subsampling);
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
StatusOr<Image> ImageBufferLoader<T>::LoadFromInterleavedBuffer(
    ReadOnlySpan<T> buffer, uint32_t width, uint32_t height, uint8_t bit_depth,
    const ws::imaging::pixel::PixelFormat pixel_format) {
  const ws::imaging::pixel::PixelFormatDetails* pixel_format_details =
      ws::imaging::pixel::PixelFormatConstraints::GetFormat(pixel_format);
  if (!pixel_format_details)
    return Status(StatusCode::kBadRequest,
                  "Unsupported pixel format " +
                      ws::imaging::pixel::PixelFormatToString(pixel_format));

  return ImageBufferLoader<T>::LoadFromInterleavedBuffer(
      buffer, width, height, bit_depth, pixel_format_details);
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
StatusOr<Image> ImageBufferLoader<T>::LoadFromPlanarBuffer(
    ReadOnlySpan<T> buffer, uint32_t width, uint32_t height, uint8_t bit_depth,
    const ws::imaging::pixel::PixelFormatDetails* pixel_format_details) {
  if (width == 0)
    return Status(StatusCode::kBadRequest, "Width must be greater than 0");
  if (height == 0)
    return Status(StatusCode::kBadRequest, "Height must be greater than 0");
  if (buffer.Empty())
    return Status(StatusCode::kBadRequest, "Buffer must not be empty");
  if (DetermineImageBufferType(bit_depth) != ImageBufferTypeOf<T>::value)
    return Status(StatusCode::kBadRequest,
                  "Bit depth does not match buffer type");

  if (!pixel_format_details)
    return Status(StatusCode::kBadRequest,
                  "Pixel format details must not be null");

  if ((pixel_format_details->layout &
       ws::imaging::pixel::PixelLayoutFlag::kPlanar) ==
      static_cast<ws::imaging::pixel::PixelLayoutFlag>(0))
    return Status(StatusCode::kBadRequest,
                  "Pixel format details must indicate planar layout");

  Array<Point> dimensions =
      ws::imaging::pixel::PixelFormatConstraints::GetDimensions(
          width, height, pixel_format_details->num_components,
          pixel_format_details->chroma_subsampling,
          pixel_format_details->HasAlpha());
  if (dimensions.Empty())
    return Status(StatusCode::kBadRequest,
                  "Unsupported dimensions for the given pixel format");

  ws::ReadOnlySpan<uint8_t> components_order =
      pixel_format_details->components_order;
  if (buffer.Length() % components_order.Length() != 0)
    return Status(StatusCode::kBadRequest,
                  "Buffer length must be divisible by components order length");

  Array<ImageComponent> components(pixel_format_details->num_components);
  const T* buffer_ptr = static_cast<const T*>(buffer);
  for (size_t i = 0; i < components_order.Length(); i++) {
    uint8_t c = components_order[i];
    ASSIGN_OR_RETURN(components[c],
                     std::move(ImageComponent::Create<T>(
                         static_cast<uint32_t>(dimensions[c].x),
                         dimensions[c].x * dimensions[c].y, bit_depth,
                         c == pixel_format_details->alpha_index)));
    T* comp_buffer(components[c].Buffer<T>());
    std::memcpy(comp_buffer, buffer_ptr, components[c].Length() * sizeof(T));
    buffer_ptr += components[c].Length();
  }

  return Image::Create(std::move(components), width, height,
                       pixel_format_details->color_space,
                       pixel_format_details->chroma_subsampling);
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
StatusOr<Image> ImageBufferLoader<T>::LoadFromPlanarBuffer(
    ReadOnlySpan<T> buffer, uint32_t width, uint32_t height, uint8_t bit_depth,
    const ws::imaging::pixel::PixelFormat pixel_format) {
  const ws::imaging::pixel::PixelFormatDetails* pixel_format_details =
      ws::imaging::pixel::PixelFormatConstraints::GetFormat(pixel_format);
  if (!pixel_format_details)
    return Status(StatusCode::kBadRequest,
                  "Unsupported pixel format " +
                      ws::imaging::pixel::PixelFormatToString(pixel_format));

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
