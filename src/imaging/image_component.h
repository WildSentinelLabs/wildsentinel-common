#pragma once

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <format>

#include "array.h"
#include "idisposable.h"
#include "imaging/image_buffer_type.h"
#include "imaging/pixel/pixel_allowed_types.h"
#include "span.h"
namespace ws {
namespace imaging {

class IImageComponent {
 public:
  ~IImageComponent() = default;

  virtual const void* Buffer() const = 0;

  virtual ImageBufferType GetBufferType() const = 0;

  virtual uint32_t Width() const = 0;

  virtual uint32_t Height() const = 0;

  virtual size_t Length() const = 0;

  virtual uint8_t BitDepth() const = 0;

  virtual bool IsAlpha() const = 0;

  virtual bool Empty() const = 0;

  virtual bool IsValid() const = 0;

  virtual std::string ToString() const = 0;
};

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
class ImageComponent : public IImageComponent {
 public:
  ImageComponent();

  ImageComponent(Array<T>&& buffer, uint32_t width, uint32_t height,
                 uint8_t bit_depth, bool is_alpha = false);

  ~ImageComponent() = default;

  ImageComponent(ImageComponent&& other) noexcept;

  ImageComponent(const ImageComponent&) = delete;

  const void* Buffer() const override;

  ImageBufferType GetBufferType() const override;

  uint32_t Width() const override;

  uint32_t Height() const override;

  size_t Length() const override;

  uint8_t BitDepth() const override;

  bool IsAlpha() const override;

  bool Empty() const override;

  bool IsValid() const override;

  std::string ToString() const override;

  ImageComponent& operator=(const ImageComponent&) = delete;

  ImageComponent& operator=(ImageComponent&& other) noexcept;

  const T& operator[](std::size_t index) const;

  operator const T*() const;

 private:
  Array<T> buffer_;
  ImageBufferType buffer_type_;
  uint32_t width_;
  uint32_t height_;
  uint8_t bit_depth_;
  bool is_alpha_;
};
}  // namespace imaging
}  // namespace ws
