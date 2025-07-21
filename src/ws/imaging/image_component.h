#pragma once

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>

#include "ws/array.h"
#include "ws/imaging/image_buffer_type.h"
#include "ws/imaging/pixel/pixel_allowed_types.h"
#include "ws/span.h"
#include "ws/status/status_or.h"
#include "ws/string/format.h"
#include "ws/types.h"
namespace ws {
namespace imaging {
class ImageComponent {
 public:
  template <ws::imaging::IsAllowedPixelNumericType T>
  static StatusOr<ImageComponent> Create(uint32_t width, offset_t length,
                                         uint8_t bit_depth,
                                         bool is_alpha = false);

  ImageComponent();
  ImageComponent(const ImageComponent&) = delete;
  ImageComponent(ImageComponent&&) noexcept;
  ImageComponent& operator=(const ImageComponent&) = delete;
  ImageComponent& operator=(ImageComponent&&) noexcept;

  template <ws::imaging::IsAllowedPixelNumericType T>
  operator T*() const;

  ~ImageComponent();

  uint32_t Width() const;
  size_t Length() const;
  size_t Height() const;
  uint8_t BitDepth() const;
  bool IsAlpha() const;
  bool Empty() const;
  bool IsValid() const;
  std::string ToString() const;
  ImageBufferType GetBufferType() const;
  template <ws::imaging::IsAllowedPixelNumericType T>
  T* Buffer() const;

 private:
  ImageComponent(void* buffer, uint32_t width, offset_t length,
                 uint8_t bit_depth, bool is_alpha, ImageBufferType type);

  void FreeBuffer();
  void Dispose();

  void* buffer_;
  uint32_t width_;
  uint32_t height_;
  offset_t length_;
  uint8_t bit_depth_;
  bool is_alpha_;
  ImageBufferType buffer_type_;
};

// ============================================================================
// Implementation details for ImageComponent
// ============================================================================

inline uint32_t ImageComponent::Width() const { return width_; }

inline size_t ImageComponent::Length() const { return length_; }

inline size_t ImageComponent::Height() const { return height_; }

inline uint8_t ImageComponent::BitDepth() const { return bit_depth_; }

inline bool ImageComponent::IsAlpha() const { return is_alpha_; }

inline bool ImageComponent::Empty() const { return buffer_ == nullptr; }

inline bool ImageComponent::IsValid() const {
  return !Empty() && length_ != 0 && width_ != 0 && bit_depth_ != 0 &&
         buffer_type_ != ImageBufferType::kUnknown;
}

inline ImageBufferType ImageComponent::GetBufferType() const {
  return buffer_type_;
}

// TODO: VALIDATE BUFFER (IF)?
template <ws::imaging::IsAllowedPixelNumericType T>
inline T* ImageComponent::Buffer() const {
  assert(buffer_ != nullptr && "Buffer is null");
  assert(ImageBufferTypeOf<T>::value == buffer_type_ &&
         "Incorrect buffer type");
  return static_cast<T*>(buffer_);
}

template <ws::imaging::IsAllowedPixelNumericType T>
inline ImageComponent::operator T*() const {
  return Buffer<T>();
}

}  // namespace imaging
}  // namespace ws
