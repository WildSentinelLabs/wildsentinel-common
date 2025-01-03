#include "imaging/pixel_buffer.h"

template <typename T>
PixelBuffer<T>::PixelBuffer(T* buffer, const size_t& size,
                            const uint8_t& bit_depth, const uint32_t& width,
                            const uint32_t& height, const PixelFormat& format,
                            const uint8_t alignment, const bool is_planar)
    : pixel_source_(buffer),
      pixel_format_(format),
      size_(size),
      bit_depth_(bit_depth),
      width_(width),
      height_(height),
      alignment_(alignment),
      is_planar_(is_planar) {
  if (!IsValid()) {
    throw std::invalid_argument(
        "Parameters must be non-null and greater than zero");
  }
}

template <typename T>
PixelBuffer<T>::~PixelBuffer() {
  Dispose();
}

template <typename T>
const void* PixelBuffer<T>::GetPixelSource() const {
  return pixel_source_;
}

template <typename T>
PixelFormat PixelBuffer<T>::GetPixelFormat() const {
  return pixel_format_;
}

template <typename T>
size_t PixelBuffer<T>::GetSize() const {
  return size_;
}

template <typename T>
uint32_t PixelBuffer<T>::GetWidth() const {
  return width_;
}

template <typename T>
uint32_t PixelBuffer<T>::GetHeight() const {
  return height_;
}

template <typename T>
uint8_t PixelBuffer<T>::GetBitDepth() const {
  return bit_depth_;
}

template <typename T>
uint8_t PixelBuffer<T>::GetAlignment() const {
  return alignment_;
}

template <typename T>
bool PixelBuffer<T>::IsPlanar() const {
  return is_planar_;
}

template <typename T>
bool PixelBuffer<T>::IsValid() const {
  return pixel_source_ && size_ != 0 && width_ != 0 && height_ != 0 &&
         bit_depth_ != 0 && pixel_format_.IsValid();
}

template <typename T>
IPixelBuffer* PixelBuffer<T>::ToInterleaved() const {
  ColorSpace color_space = pixel_format_.color_space;
  uint8_t num_comps = pixel_format_.num_components;
  if (!is_planar_ || color_space == ColorSpace::kGRAYSCALE || num_comps == 1) {
    return nullptr;
  } else if (color_space == ColorSpace::kUNSUPPORTED) {
    return nullptr;
  } else if (color_space == ColorSpace::kYUV ||
             color_space == ColorSpace::kYCbCr ||
             color_space == ColorSpace::kEYCC) {
    return nullptr;
  } else {
    return nullptr;
  }
}

template <typename T>
std::string PixelBuffer<T>::ToString() const {
  std::ostringstream ss;
  ss << "PixelBuffer" << "(" << (is_planar_ ? "Planar" : "Interleaved")
     << "):" << std::endl
     << pixel_format_.ToString() << std::endl
     << " Width: " << width_ << std::endl
     << " Height: " << height_ << std::endl
     << " Size: " << size_ << std::endl
     << " Depth: " << static_cast<int>(bit_depth_) << " bits" << std::endl
     << " Alignment: " << static_cast<int>(alignment_);
  return ss.str();
}

template <typename T>
void PixelBuffer<T>::Dispose() {
  if (pixel_source_) {
    delete[] pixel_source_;
    pixel_source_ = nullptr;
  }

  width_ = 0;
  height_ = 0;
  size_ = 0;
}

std::ostream& operator<<(std::ostream& os, const IPixelBuffer& buffer) {
  std::string str = buffer.ToString();
  os << str;
  return os;
}

template class PixelBuffer<uint8_t>;
template class PixelBuffer<uint16_t>;
template class PixelBuffer<uint32_t>;
template class PixelBuffer<int16_t>;
template class PixelBuffer<int32_t>;
