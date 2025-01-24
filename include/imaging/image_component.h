#pragma once

#include <cstddef>
#include <cstdint>
#include <sstream>

class ImageComponent {
 public:
  enum class BufferType { kUINT8, kUINT16, kINT16, kUINT32, kINT32, kUNKNOWN };

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

  void Dispose();

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

template <typename T>
class TypedImageComponent : public ImageComponent {
 public:
  TypedImageComponent(T* buffer, uint32_t width, uint32_t height,
                      uint8_t bit_depth, bool is_alpha = false);

  ~TypedImageComponent();

  const T* GetTypedBuffer() const;
};
