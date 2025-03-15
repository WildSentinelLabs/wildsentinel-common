#pragma once

#include <cstddef>
#include <cstdint>
#include <sstream>

#include "base/idisposable.h"
#include "imaging/pixel/pixel_allowed_types.h"
namespace ws {
namespace imaging {

class ImageComponent : public IDisposable {
 public:
  enum class BufferType : int8_t {
    kUInt8,
    kUInt16,
    kInt16,
    kUInt32,
    kInt32,
    kUnknown
  };

  ImageComponent(void* buffer, uint32_t width, uint32_t height,
                 uint8_t bit_depth, bool is_alpha = false);

  ~ImageComponent();

  static BufferType DetermineBufferType(uint8_t bit_depth);

  const void* GetBuffer() const;

  uint32_t GetWidth() const;

  uint32_t GetHeight() const;

  size_t GetSize() const;

  uint8_t GetBitDepth() const;

  bool IsAlpha() const;

  BufferType GetBufferType() const;

  bool IsValid() const;

  std::string ToString() const;

  void Dispose() override;

 protected:
  void* buffer_;
  BufferType buffer_type_;

 private:
  uint32_t width_;
  uint32_t height_;
  uint8_t bit_depth_;
  bool is_alpha_;
};

std::ostream& operator<<(std::ostream& os,
                         const ImageComponent& image_component);

std::string BufferTypeToString(ImageComponent::BufferType type);

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
class TypedImageComponent : public ImageComponent {
 public:
  TypedImageComponent(T* buffer, uint32_t width, uint32_t height,
                      uint8_t bit_depth, bool is_alpha = false);

  ~TypedImageComponent();

  const T* GetTypedBuffer() const;
};
}  // namespace imaging
}  // namespace ws
