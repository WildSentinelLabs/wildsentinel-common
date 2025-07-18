#include "imaging/image.h"
namespace ws {
namespace imaging {
StatusOr<Image> Image::Create(Array<ImageComponent>&& components,
                              uint32_t width, uint32_t height,
                              ColorSpace color_space,
                              ChromaSubsampling chroma_subsampling) {
  if (components.Empty()) {
    return Status(StatusCode::kBadRequest, "Components array cannot be empty");
  }

  for (auto& component : components) {
    if (!component.IsValid())
      return Status(StatusCode::kBadRequest, "Invalid image component");
  }

  if (width == 0) {
    return Status(StatusCode::kBadRequest, "Width must be greater than 0");
  }
  if (height == 0) {
    return Status(StatusCode::kBadRequest, "Height must be greater than 0");
  }
  if (color_space == ColorSpace::kUnsupported) {
    return Status(StatusCode::kBadRequest, "Color space cannot be unsupported");
  }
  if (chroma_subsampling == ChromaSubsampling::kUnsupported) {
    return Status(StatusCode::kBadRequest,
                  "Chroma subsampling cannot be unsupported");
  }

  return Image(std::move(components), width, height, color_space,
               chroma_subsampling);
}

Image::Image()
    : components_(Array<ImageComponent>()),
      context_(ImageContext()),
      width_(0),
      height_(0),
      color_space_(ColorSpace::kUnsupported),
      chroma_subsampling_(ChromaSubsampling::kUnsupported) {}

Image::Image(Image&& other) noexcept
    : components_(std::move(other.components_)),
      context_(std::move(other.context_)),
      width_(other.width_),
      height_(other.height_),
      color_space_(other.color_space_),
      chroma_subsampling_(other.chroma_subsampling_) {
  other.components_ = Array<ImageComponent>();
  other.context_ = ImageContext();
  other.width_ = 0;
  other.height_ = 0;
  other.color_space_ = ColorSpace::kUnsupported;
  other.chroma_subsampling_ = ChromaSubsampling::kUnsupported;
}

Image& Image::operator=(Image&& other) noexcept {
  if (this != &other) {
    components_ = std::move(other.components_);
    context_ = std::move(other.context_);
    width_ = other.width_;
    height_ = other.height_;
    color_space_ = other.color_space_;
    chroma_subsampling_ = other.chroma_subsampling_;

    other.components_ = Array<ImageComponent>();
    other.context_ = ImageContext();
    other.width_ = 0;
    other.height_ = 0;
    other.color_space_ = ColorSpace::kUnsupported;
    other.chroma_subsampling_ = ChromaSubsampling::kUnsupported;
  }

  return *this;
}

void Image::LoadContext(const ImageContext& context) {
  context_.Clear();
  for (const auto& [key, value] : context) {
    context_.Add(key, value);
  }
}

bool Image::HasAlpha() const {
  for (auto& component : components_) {
    if (component.IsAlpha()) return true;
  }

  return false;
}

bool Image::IsValid() const {
  if (components_.Empty() || width_ == 0 || height_ == 0 ||
      color_space_ == ColorSpace::kUnsupported ||
      chroma_subsampling_ == ChromaSubsampling::kUnsupported)
    return false;

  for (auto& component : components_) {
    if (!component.IsValid()) return false;
  }

  return true;
}

std::string Image::ToString() const {
  std::string result = Format(
      "Image(Width: {} Height: {} ColorSpace: {} ChromaSubsampling: "
      "{})",
      width_, height_, ColorSpaceToString(color_space_),
      ChromaSubsamplingToString(chroma_subsampling_));

  for (uint8_t c = 0; c < components_.Length(); ++c) {
    result += Format("=> Component {}: {}\n", static_cast<int>(c),
                     components_[c].ToString());
  }

  result += Format("=> Context: {}", context_.ToString());
  return result;
}

Image::Image(Array<ImageComponent>&& components, uint32_t width,
             uint32_t height, ColorSpace color_space,
             ChromaSubsampling chroma_subsampling)
    : components_(std::move(components)),
      context_(ImageContext()),
      width_(width),
      height_(height),
      color_space_(color_space),
      chroma_subsampling_(chroma_subsampling) {}
}  // namespace imaging
}  // namespace ws
