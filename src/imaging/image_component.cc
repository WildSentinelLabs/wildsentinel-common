#include "imaging/image_component.h"
namespace ws {
namespace imaging {

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
ImageComponent<T>::ImageComponent()
    : buffer_(Array<T>::Empty()),
      buffer_type_(ImageBufferType::kUnknown),
      width_(0),
      height_(0),
      bit_depth_(0),
      is_alpha_(false) {}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
ImageComponent<T>::ImageComponent(Array<T>&& buffer, uint32_t width,
                                  uint32_t height, uint8_t bit_depth,
                                  bool is_alpha)
    : buffer_(std::move(buffer)),
      width_(width),
      height_(height),
      bit_depth_(bit_depth),
      is_alpha_(is_alpha),
      buffer_type_(DetermineImageBufferType(bit_depth)) {
  if (!IsValid())
    throw std::invalid_argument("Invalid ImageComponent parameters");
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
ImageComponent<T>::ImageComponent(ImageComponent<T>&& other) noexcept
    : buffer_(std::move(other.buffer_)),
      buffer_type_(other.buffer_type_),
      width_(other.width_),
      height_(other.height_),
      bit_depth_(other.bit_depth_),
      is_alpha_(other.is_alpha_) {
  other.buffer_ = Array<T>::Empty();
  other.buffer_type_ = ImageBufferType::kUnknown;
  other.width_ = 0;
  other.height_ = 0;
  other.bit_depth_ = 0;
  other.is_alpha_ = false;
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
ImageComponent<T> ImageComponent<T>::Empty() {
  return ImageComponent<T>();
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
const void* ImageComponent<T>::Buffer() const {
  return static_cast<const T*>(buffer_);
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
uint32_t ImageComponent<T>::Width() const {
  return width_;
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
uint32_t ImageComponent<T>::Height() const {
  return height_;
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
size_t ImageComponent<T>::Length() const {
  return width_ * height_;
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
uint8_t ImageComponent<T>::BitDepth() const {
  return bit_depth_;
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
bool ImageComponent<T>::IsAlpha() const {
  return is_alpha_;
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
bool ImageComponent<T>::IsValid() const {
  return !(buffer_.IsEmpty() || width_ == 0 || height_ == 0 || bit_depth_ == 0);
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
ImageBufferType ImageComponent<T>::GetBufferType() const {
  return buffer_type_;
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
std::string ImageComponent<T>::ToString() const {
  return std::format(
      "ImageComponent<{}>( Width: {} Height: {} Bit Depth: {} Alpha: {} )",
      ImageBufferTypeToString(buffer_type_), width_, height_,
      static_cast<int>(bit_depth_), (is_alpha_ ? "True" : "False"));
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
ImageComponent<T>& ImageComponent<T>::operator=(
    ImageComponent<T>&& other) noexcept {
  if (this != &other) {
    buffer_ = std::move(other.buffer_);
    buffer_type_ = other.buffer_type_;
    width_ = other.width_;
    height_ = other.height_;
    bit_depth_ = other.bit_depth_;
    is_alpha_ = other.is_alpha_;

    other.buffer_ = Array<T>::Empty();
    other.buffer_type_ = ImageBufferType::kUnknown;
    other.width_ = 0;
    other.height_ = 0;
    other.bit_depth_ = 0;
    other.is_alpha_ = false;
  }

  return *this;
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
const T& ImageComponent<T>::operator[](std::size_t index) const {
  assert(index < buffer_.Length() && "Array index out of range");
  return buffer_[index];
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
ImageComponent<T>::operator const T*() const {
  return buffer_;
}

template class ImageComponent<int8_t>;
template class ImageComponent<uint8_t>;
template class ImageComponent<int16_t>;
template class ImageComponent<uint16_t>;
template class ImageComponent<int32_t>;
template class ImageComponent<uint32_t>;
}  // namespace imaging
}  // namespace ws
