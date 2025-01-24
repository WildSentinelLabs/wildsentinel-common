#include "imaging/image_component.h"

ImageComponent::ImageComponent(void* buffer, uint32_t width, uint32_t height,
                               uint8_t bit_depth, bool is_alpha)
    : buffer_(buffer),
      width_(width),
      height_(height),
      bit_depth_(bit_depth),
      is_alpha_(is_alpha) {
  if (!IsValid())
    throw std::invalid_argument("Invalid ImageComponent parameters");
}

ImageComponent::~ImageComponent() { Dispose(); }

ImageComponent::BufferType ImageComponent::DetermineBufferType(
    uint8_t bit_depth) {
  if (bit_depth >= 2 && bit_depth <= 8) {
    return BufferType::kUINT8;
  } else if (bit_depth >= 9 && bit_depth <= 12) {
    return BufferType::kINT16;
  } else if (bit_depth >= 13 && bit_depth <= 16) {
    return BufferType::kUINT16;
  } else if (bit_depth >= 17 && bit_depth <= 24) {
    return BufferType::kINT32;
  } else if (bit_depth >= 25 && bit_depth <= 32) {
    return BufferType::kUINT32;
  } else {
    return BufferType::kUNKNOWN;
  }
}

const void* ImageComponent::GetBuffer() const { return buffer_; }

uint32_t ImageComponent::GetWidth() const { return width_; }

uint32_t ImageComponent::GetHeight() const { return height_; }

size_t ImageComponent::GetSize() const { return width_ * height_; }

uint8_t ImageComponent::GetBitDepth() const { return bit_depth_; }

bool ImageComponent::IsAlpha() const { return is_alpha_; }

bool ImageComponent::IsValid() const {
  return !(buffer_ == nullptr || width_ == 0 || height_ == 0 ||
           bit_depth_ == 0);
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
  if (buffer_) {
    switch (buffer_type_) {
      case ImageComponent::BufferType::kUINT8:
        delete[] (uint8_t*)buffer_;
        break;
      case ImageComponent::BufferType::kUINT16:
        delete[] (uint16_t*)buffer_;
        break;
      case ImageComponent::BufferType::kINT16:
        delete[] (int16_t*)buffer_;
        break;
      case ImageComponent::BufferType::kUINT32:
        delete[] (uint32_t*)buffer_;
        break;
      case ImageComponent::BufferType::kINT32:
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
    case ImageComponent::BufferType::kUINT8:
      return "UINT8";
    case ImageComponent::BufferType::kUINT16:
      return "UINT16";
    case ImageComponent::BufferType::kINT16:
      return "INT16";
    case ImageComponent::BufferType::kUINT32:
      return "UINT32";
    case ImageComponent::BufferType::kINT32:
      return "INT32";
    default:
      return "UNKNOWN";
  }
}

template class TypedImageComponent<uint8_t>;
template class TypedImageComponent<int16_t>;
template class TypedImageComponent<uint16_t>;
template class TypedImageComponent<int32_t>;
template class TypedImageComponent<uint32_t>;

template <typename T>
TypedImageComponent<T>::TypedImageComponent(T* buffer, uint32_t width,
                                            uint32_t height, uint8_t bit_depth,
                                            bool is_alpha)
    : ImageComponent(static_cast<void*>(buffer), width, height, bit_depth,
                     is_alpha) {}

template <typename T>
TypedImageComponent<T>::~TypedImageComponent() {
  Dispose();
}

template <typename T>
const T* TypedImageComponent<T>::GetTypedBuffer() const {
  return static_cast<const T*>(buffer_);
}
