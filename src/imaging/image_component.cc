#include "imaging/image_component.h"
namespace ws {
namespace imaging {

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
ImageComponent<T>::~ImageComponent() {
  Dispose();
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
const void* ImageComponent<T>::Buffer() const {
  if (disposed_.load()) throw disposed_object_exception();
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
  return !disposed_.load() && !(buffer_ == nullptr || width_ == 0 ||
                                height_ == 0 || bit_depth_ == 0);
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
ImageBufferType ImageComponent<T>::GetBufferType() const {
  return buffer_type_;
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
std::string ImageComponent<T>::ToString() const {
  std::ostringstream ss;
  ss << "ImageComponent<" << ImageBufferTypeToString(buffer_type_) << ">("
     << " Width: " << width_ << " Height: " << height_
     << ", Bit Depth: " << static_cast<int>(bit_depth_)
     << ", Alpha: " << (is_alpha_ ? "True" : "False") << ")";
  return ss.str();
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
void ImageComponent<T>::Dispose() {
  if (disposed_.exchange(true)) return;
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
ImageComponent<T>::operator const T*() const {
  return buffer_;
}

template <ws::imaging::pixel::IsAllowedPixelNumericType T>
const T& ImageComponent<T>::operator[](std::size_t index) const {
  assert(index < buffer_.Length() && "Array index out of range");
  return buffer_[index];
}

std::ostream& operator<<(std::ostream& os,
                         const IImageComponent& image_component) {
  os << image_component.ToString();
  return os;
}

template class ImageComponent<int8_t>;
template class ImageComponent<uint8_t>;
template class ImageComponent<int16_t>;
template class ImageComponent<uint16_t>;
template class ImageComponent<int32_t>;
template class ImageComponent<uint32_t>;
}  // namespace imaging
}  // namespace ws
