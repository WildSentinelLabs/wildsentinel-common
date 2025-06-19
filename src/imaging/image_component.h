#pragma once

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>

#include "array.h"
#include "format.h"
#include "idisposable.h"
#include "imaging/image_buffer_type.h"
#include "imaging/pixel/pixel_allowed_types.h"
#include "span.h"
#include "status/status_or.h"
#include "types.h"
namespace ws {
namespace imaging {

class ImageComponent {
 public:
  template <ws::imaging::pixel::IsAllowedPixelNumericType T>
  static StatusOr<ImageComponent> Create(uint32_t width, offset_t length,
                                         uint8_t bit_depth,
                                         bool is_alpha = false);

  ImageComponent();
  ImageComponent(const ImageComponent&) = delete;
  ImageComponent(ImageComponent&& other) noexcept;
  ImageComponent& operator=(const ImageComponent&) = delete;
  ImageComponent& operator=(ImageComponent&& other) noexcept;

  ~ImageComponent();

  uint32_t Width() const;
  size_t Length() const;
  uint8_t BitDepth() const;
  bool IsAlpha() const;
  bool Empty() const;
  bool IsValid() const;
  std::string ToString() const;
  ImageBufferType GetBufferType() const;
  template <ws::imaging::pixel::IsAllowedPixelNumericType T>
  T* Buffer() const;
  template <ws::imaging::pixel::IsAllowedPixelNumericType T>
  operator T*() const;

 private:
  ImageComponent(void* buffer, uint32_t width, offset_t length,
                 uint8_t bit_depth, bool is_alpha, ImageBufferType type);

  void FreeBuffer();
  void Dispose();

  void* buffer_;
  uint32_t width_;
  offset_t length_;
  uint8_t bit_depth_;
  bool is_alpha_;
  ImageBufferType buffer_type_;
};
}  // namespace imaging
}  // namespace ws
