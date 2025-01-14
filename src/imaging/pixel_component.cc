#include "imaging/pixel_component.h"

template <typename T>
PixelComponent<T>::PixelComponent(T* buffer, uint32_t width, uint32_t height,
                                  uint8_t bit_depth)
    : buffer_(buffer), width_(width), height_(height), bit_depth_(bit_depth) {
  if (!IsValid())
    throw std::invalid_argument("Invalid PixelComponent parameters");
}

template <typename T>
PixelComponent<T>::~PixelComponent() {
  Dispose();
}

template <typename T>
const void* PixelComponent<T>::GetBuffer() const {
  return static_cast<void*>(buffer_);
}

template <typename T>
const uint32_t& PixelComponent<T>::GetWidth() const {
  return width_;
}

template <typename T>
const uint32_t& PixelComponent<T>::GetHeight() const {
  return width_;
}

template <typename T>
const size_t PixelComponent<T>::GetSize() const {
  return width_ * height_;
}

template <typename T>
const uint8_t PixelComponent<T>::GetBitDepth() const {
  return bit_depth_;
}

template <typename T>
bool PixelComponent<T>::IsValid() const {
  return !(buffer_ == nullptr || width_ == 0 || height_ == 0 ||
           bit_depth_ == 0);
}

template <typename T>
std::string PixelComponent<T>::ToString() const {
  std::ostringstream ss;
  ss << "PixelComponent("
     << " Width: " << width_ << " Height: " << height_
     << ", Bit Depth: " << static_cast<int>(bit_depth_) << ")";
  return ss.str();
}

template <typename T>
void PixelComponent<T>::Dispose() {
  if (buffer_) {
    delete[] buffer_;
    buffer_ = nullptr;
  }
}

template <typename T>
const T* PixelComponent<T>::GetTypedBuffer() const {
  return buffer_;
}

template class PixelComponent<uint8_t>;
template class PixelComponent<uint16_t>;
template class PixelComponent<uint32_t>;
template class PixelComponent<int16_t>;
template class PixelComponent<int32_t>;

std::ostream& operator<<(std::ostream& os,
                         const IPixelComponent& pixel_component) {
  os << pixel_component.ToString();
  return os;
}
