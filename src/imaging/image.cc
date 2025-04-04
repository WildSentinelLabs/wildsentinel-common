#include "imaging/image.h"
namespace ws {
namespace imaging {

Image::Image()
    : components_(Array<std::unique_ptr<IImageComponent>>::Empty()),
      context_(ImageContext()),
      width_(0),
      height_(0),
      color_space_(ColorSpace::kUnsupported),
      chroma_subsampling_(ChromaSubsampling::kUnsupported) {}

Image::Image(Array<std::unique_ptr<IImageComponent>>&& components,
             uint32_t width, uint32_t height, ColorSpace color_space,
             ChromaSubsampling chroma_subsampling)
    : components_(std::move(components)),
      context_(ImageContext()),
      width_(width),
      height_(height),
      color_space_(color_space),
      chroma_subsampling_(chroma_subsampling) {
  if (!IsValid())
    throw std::invalid_argument(
        "Invalid parameters: buffers, dimensions, and bit depth must be "
        "non-null and non-zero");
}

Image::Image(Image&& other) noexcept
    : components_(std::move(other.components_)),
      context_(std::move(other.context_)),
      width_(other.width_),
      height_(other.height_),
      color_space_(other.color_space_),
      chroma_subsampling_(other.chroma_subsampling_) {
  other.components_ = Array<std::unique_ptr<IImageComponent>>::Empty();
  other.context_ = ImageContext();
  other.width_ = 0;
  other.height_ = 0;
  other.color_space_ = ColorSpace::kUnsupported;
  other.chroma_subsampling_ = ChromaSubsampling::kUnsupported;
}

Image Image::Empty() { return Image(); }

void Image::LoadContext(const ImageContext& context) {
  context_.Clear();
  for (const auto& [key, value] : context) {
    context_.Add(key, value);
  }
}

ImageContext Image::Context() const { return context_; }

uint8_t Image::NumComponents() const { return components_.Length(); }

const IImageComponent* Image::GetComponent(uint8_t comp_num) const {
  return components_[comp_num].get();
}

Array<const IImageComponent*> Image::Components() const {
  Array<const IImageComponent*> components_array(components_.Length());
  for (size_t i = 0; i < components_.Length(); i++) {
    components_array[i] = components_[i].get();
  }

  return components_array;
}

uint32_t Image::Width() const { return width_; }

uint32_t Image::Height() const { return height_; }

ColorSpace Image::GetColorSpace() const { return color_space_; }

ChromaSubsampling Image::GetChromaSubsampling() const {
  return chroma_subsampling_;
}

bool Image::HasAlpha() const {
  for (uint8_t c = 0; c < components_.Length(); c++) {
    if (components_[c]->IsAlpha()) return true;
  }

  return false;
}

bool Image::IsValid() const {
  bool result = !(components_.IsEmpty() || width_ == 0 || height_ == 0 ||
                  color_space_ == ColorSpace::kUnsupported ||
                  chroma_subsampling_ == ChromaSubsampling::kUnsupported);
  for (uint8_t c = 0; c < components_.Length(); ++c) {
    result = result && components_[c]->IsValid();
  }

  return result;
}

std::string Image::ToString() const {
  std::string result = std::format(
      "Image:\n Width: {}\n Height: {}\n ColorSpace: {}\n ChromaSubsampling: "
      "{}\n",
      width_, height_, ColorSpaceToString(color_space_),
      ChromaSubsamplingToString(chroma_subsampling_));

  for (uint8_t c = 0; c < components_.Length(); ++c) {
    result += std::format(" Component {}: {}\n", static_cast<int>(c),
                          components_[c]->ToString());
  }

  return result;
}

Image& Image::operator=(Image&& other) noexcept {
  if (this != &other) {
    components_ = std::move(other.components_);
    context_ = std::move(other.context_);
    width_ = other.width_;
    height_ = other.height_;
    color_space_ = other.color_space_;
    chroma_subsampling_ = other.chroma_subsampling_;

    other.components_ = Array<std::unique_ptr<IImageComponent>>::Empty();
    other.context_ = ImageContext();
    other.width_ = 0;
    other.height_ = 0;
    other.color_space_ = ColorSpace::kUnsupported;
    other.chroma_subsampling_ = ChromaSubsampling::kUnsupported;
  }

  return *this;
}
}  // namespace imaging
}  // namespace ws
