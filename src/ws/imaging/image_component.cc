#include "ws/imaging/image_component.h"
namespace ws {
namespace imaging {
template <ws::imaging::IsAllowedPixelNumericType T>
StatusOr<ImageComponent> ImageComponent::Create(uint32_t width, offset_t length,
                                                uint8_t bit_depth,
                                                bool is_alpha) {
  static_assert(
      std::is_same_v<
          T, typename ImageBufferTType<ImageBufferTypeOf<T>::value>::type>,
      "Unsupported buffer type");

  if (length <= 0)
    return Status(StatusCode::kBadRequest, "Length must be greater than 0");
  if (width <= 0)
    return Status(StatusCode::kBadRequest, "Width must be greater than 0");
  if (width >= length)
    return Status(StatusCode::kBadRequest, "Width must be less than Length");
  if (bit_depth <= 0)
    return Status(StatusCode::kBadRequest, "Bit depth must be greater than 0");

  if (DetermineImageBufferType(bit_depth) != ImageBufferTypeOf<T>::value)
    return Status(StatusCode::kBadRequest,
                  "Bit depth does not match buffer type");

  void* buf = static_cast<void*>(new T[length]);
  return ImageComponent(buf, width, length, bit_depth, is_alpha,
                        ImageBufferTypeOf<T>::value);
}

ImageComponent::ImageComponent()
    : buffer_(nullptr),
      width_(0),
      length_(0),
      height_(0),
      bit_depth_(0),
      is_alpha_(false),
      buffer_type_(ImageBufferType::kUnknown) {}

ImageComponent::ImageComponent(ImageComponent&& other) noexcept
    : buffer_(other.buffer_),
      width_(other.width_),
      length_(other.length_),
      height_(other.height_),
      bit_depth_(other.bit_depth_),
      is_alpha_(other.is_alpha_),
      buffer_type_(other.buffer_type_) {
  other.buffer_ = nullptr;
  other.width_ = 0;
  other.length_ = 0;
  other.height_ = 0;
  other.bit_depth_ = 0;
  other.is_alpha_ = false;
  other.buffer_type_ = ImageBufferType::kUnknown;
}

ImageComponent& ImageComponent::operator=(ImageComponent&& other) noexcept {
  if (this != &other) {
    buffer_ = other.buffer_;
    width_ = other.width_;
    length_ = other.length_;
    height_ = other.height_;
    bit_depth_ = other.bit_depth_;
    is_alpha_ = other.is_alpha_;
    buffer_type_ = other.buffer_type_;

    other.buffer_ = nullptr;
    other.width_ = 0;
    other.length_ = 0;
    other.height_ = 0;
    other.bit_depth_ = 0;
    other.is_alpha_ = false;
    other.buffer_type_ = ImageBufferType::kUnknown;
  }

  return *this;
}

ImageComponent::~ImageComponent() { Dispose(); }

std::string ImageComponent::ToString() const {
  return Format(
      "ImageComponent<{}>(Width: {}, Length: {}, Bit Depth: {}, Alpha: {})",
      ImageBufferTypeToString(buffer_type_), width_, length_,
      static_cast<int>(bit_depth_), (is_alpha_ ? "True" : "False"));
}

ImageComponent::ImageComponent(void* buffer, uint32_t width, offset_t length,
                               uint8_t bit_depth, bool is_alpha,
                               ImageBufferType type)
    : buffer_(buffer),
      width_(width),
      length_(length),
      height_(length / width),
      bit_depth_(bit_depth),
      is_alpha_(is_alpha),
      buffer_type_(type) {}

void ImageComponent::FreeBuffer() {
  if (!buffer_) return;
  switch (buffer_type_) {
    case ImageBufferType::kUInt8:
      delete[] static_cast<uint8_t*>(buffer_);
      break;
    case ImageBufferType::kInt16:
      delete[] static_cast<int16_t*>(buffer_);
      break;
    case ImageBufferType::kUInt16:
      delete[] static_cast<uint16_t*>(buffer_);
      break;
    case ImageBufferType::kInt32:
      delete[] static_cast<int32_t*>(buffer_);
      break;
    case ImageBufferType::kUInt32:
      delete[] static_cast<uint32_t*>(buffer_);
      break;
    default:
      break;
  }

  buffer_ = nullptr;
}

void ImageComponent::Dispose() {
  FreeBuffer();
  width_ = 0;
  length_ = 0;
  height_ = 0;
  bit_depth_ = 0;
  is_alpha_ = false;
  buffer_type_ = ImageBufferType::kUnknown;
}

template StatusOr<ImageComponent> ImageComponent::Create<int8_t>(uint32_t,
                                                                 offset_t,
                                                                 uint8_t, bool);
template StatusOr<ImageComponent> ImageComponent::Create<uint8_t>(uint32_t,
                                                                  offset_t,
                                                                  uint8_t,
                                                                  bool);
template StatusOr<ImageComponent> ImageComponent::Create<int16_t>(uint32_t,
                                                                  offset_t,
                                                                  uint8_t,
                                                                  bool);
template StatusOr<ImageComponent> ImageComponent::Create<uint16_t>(uint32_t,
                                                                   offset_t,
                                                                   uint8_t,
                                                                   bool);
template StatusOr<ImageComponent> ImageComponent::Create<int32_t>(uint32_t,
                                                                  offset_t,
                                                                  uint8_t,
                                                                  bool);
template StatusOr<ImageComponent> ImageComponent::Create<uint32_t>(uint32_t,
                                                                   offset_t,
                                                                   uint8_t,
                                                                   bool);
}  // namespace imaging
}  // namespace ws
