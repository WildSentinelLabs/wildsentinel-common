#include "imaging/image_component.h"
namespace ws {
namespace imaging {
ImageComponent::ImageComponent(void* buffer, uint32_t width, uint32_t height,
                               uint8_t bit_depth, bool is_alpha)
    : buffer_(buffer),
      width_(width),
      height_(height),
      bit_depth_(bit_depth),
      is_alpha_(is_alpha),
      buffer_type_(DetermineBufferType(bit_depth)) {
  if (!IsValid())
    throw std::invalid_argument("Invalid ImageComponent parameters");
}

ImageComponent::~ImageComponent() { Dispose(); }

ImageComponent::BufferType ImageComponent::DetermineBufferType(
    uint8_t bit_depth) {
  if (bit_depth >= 2 && bit_depth <= 8) {
    return BufferType::kUInt8;
  } else if (bit_depth >= 9 && bit_depth <= 12) {
    return BufferType::kInt16;
  } else if (bit_depth >= 13 && bit_depth <= 16) {
    return BufferType::kUInt16;
  } else if (bit_depth >= 17 && bit_depth <= 24) {
    return BufferType::kInt32;
  } else if (bit_depth >= 25 && bit_depth <= 32) {
    return BufferType::kUInt32;
  } else {
    return BufferType::kUnknown;
  }
}

const void* ImageComponent::GetBuffer() const {
  if (disposed_.load()) throw disposed_object_exception();
  return buffer_;
}

uint32_t ImageComponent::GetWidth() const { return width_; }

uint32_t ImageComponent::GetHeight() const { return height_; }

size_t ImageComponent::GetSize() const { return width_ * height_; }

uint8_t ImageComponent::GetBitDepth() const { return bit_depth_; }

bool ImageComponent::IsAlpha() const { return is_alpha_; }

bool ImageComponent::IsValid() const {
  return !disposed_.load() && !(buffer_ == nullptr || width_ == 0 ||
                                height_ == 0 || bit_depth_ == 0);
}

ImageComponent::BufferType ImageComponent::GetBufferType() const {
  return buffer_type_;
}

std::string ImageComponent::ToString() const {
  std::ostringstream ss;
  ss << "ImageComponent<" << BufferTypeToString(buffer_type_) << ">("
     << " Width: " << width_ << " Height: " << height_
     << ", Bit Depth: " << static_cast<int>(bit_depth_)
     << ", Alpha: " << (is_alpha_ ? "True" : "False") << ")";
  return ss.str();
}

void ImageComponent::Dispose() {
  if (disposed_.exchange(true)) return;
  if (buffer_) {
    switch (buffer_type_) {
      case ImageComponent::BufferType::kUInt8:
        delete[] (uint8_t*)buffer_;
        break;
      case ImageComponent::BufferType::kUInt16:
        delete[] (uint16_t*)buffer_;
        break;
      case ImageComponent::BufferType::kInt16:
        delete[] (int16_t*)buffer_;
        break;
      case ImageComponent::BufferType::kUInt32:
        delete[] (uint32_t*)buffer_;
        break;
      case ImageComponent::BufferType::kInt32:
        delete[] (int32_t*)buffer_;
        break;
      default:
        break;
    }

    buffer_ = nullptr;
  }
}

std::ostream& operator<<(std::ostream& os,
                         const ImageComponent& image_component) {
  os << image_component.ToString();
  return os;
}

std::string BufferTypeToString(ImageComponent::BufferType type) {
  switch (type) {
    case ImageComponent::BufferType::kUInt8:
      return "UInt8";
    case ImageComponent::BufferType::kUInt16:
      return "UInt16";
    case ImageComponent::BufferType::kInt16:
      return "Int16";
    case ImageComponent::BufferType::kUInt32:
      return "UInt32";
    case ImageComponent::BufferType::kInt32:
      return "Int32";
    default:
      return "Unknown ";
  }
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
TypedImageComponent<T>::TypedImageComponent(T* buffer, uint32_t width,
                                            uint32_t height, uint8_t bit_depth,
                                            bool is_alpha)
    : ImageComponent(static_cast<void*>(buffer), width, height, bit_depth,
                     is_alpha) {}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
TypedImageComponent<T>::~TypedImageComponent() {
  Dispose();
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
const T* TypedImageComponent<T>::GetTypedBuffer() const {
  return static_cast<const T*>(GetBuffer());
}

template class TypedImageComponent<uint8_t>;
template class TypedImageComponent<int16_t>;
template class TypedImageComponent<uint16_t>;
template class TypedImageComponent<int32_t>;
template class TypedImageComponent<uint32_t>;
}  // namespace imaging
}  // namespace ws
