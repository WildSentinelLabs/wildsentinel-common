#include "pixel/pixel_plane.h"

template <typename T>
PixelPlane<T>::PixelPlane(T* buffer, uint32_t width, uint32_t height,
                          uint8_t bit_depth, bool is_alpha)
    : buffer_(buffer),
      width_(width),
      height_(height),
      bit_depth_(bit_depth),
      is_alpha_(is_alpha) {
  if (!IsValid()) throw std::invalid_argument("Invalid PixelPlane parameters");
}

template <typename T>
PixelPlane<T>::~PixelPlane() {
  Dispose();
}

template <typename T>
const void* PixelPlane<T>::GetBuffer() const {
  return static_cast<void*>(buffer_);
}

template <typename T>
uint32_t PixelPlane<T>::GetWidth() const {
  return width_;
}

template <typename T>
uint32_t PixelPlane<T>::GetHeight() const {
  return height_;
}

template <typename T>
size_t PixelPlane<T>::GetSize() const {
  return width_ * height_;
}

template <typename T>
uint8_t PixelPlane<T>::GetBitDepth() const {
  return bit_depth_;
}

template <typename T>
bool PixelPlane<T>::IsAlpha() const {
  return is_alpha_;
}

template <typename T>
bool PixelPlane<T>::IsValid() const {
  return !(buffer_ == nullptr || width_ == 0 || height_ == 0 ||
           bit_depth_ == 0);
}

template <typename T>
std::string PixelPlane<T>::ToString() const {
  std::ostringstream ss;
  ss << "PixelPlane("
     << " Width: " << width_ << " Height: " << height_
     << ", Bit Depth: " << static_cast<int>(bit_depth_)
     << ", Alpha: " << (is_alpha_ ? "True" : "False") << ")";
  return ss.str();
}

template <typename T>
void PixelPlane<T>::Dispose() {
  if (buffer_) {
    delete[] buffer_;
    buffer_ = nullptr;
  }
}

template <typename T>
const T* PixelPlane<T>::GetTypedBuffer() const {
  return buffer_;
}

template class PixelPlane<uint8_t>;
template class PixelPlane<int16_t>;
template class PixelPlane<uint16_t>;
template class PixelPlane<int32_t>;
template class PixelPlane<uint32_t>;

std::ostream& operator<<(std::ostream& os, const IPixelPlane& pixel_plane) {
  os << pixel_plane.ToString();
  return os;
}
