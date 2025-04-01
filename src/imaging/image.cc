#include "imaging/image.h"
namespace ws {
namespace imaging {

Image::Image(Array<std::unique_ptr<IImageComponent>>&& components,
             uint32_t width, uint32_t height, ColorSpace color_space,
             ChromaSubsampling chroma_subsampling)
    : components_(std::move(components)),
      width_(width),
      height_(height),
      color_space_(color_space),
      chroma_subsampling_(chroma_subsampling),
      context_(ImageContext()) {
  if (!IsValid())
    throw std::invalid_argument(
        "Invalid parameters: buffers, dimensions, and bit depth must be "
        "non-null and non-zero");
}

Image::~Image() { Dispose(); }

void Image::LoadContext(const ImageContext& context) {
  if (disposed_.load()) throw disposed_object_exception();
  context_.Clear();
  for (const auto& [key, value] : context) {
    context_.Add(key, value);
  }
}

ImageContext Image::Context() const {
  if (disposed_.load()) throw disposed_object_exception();
  return context_;
}

uint8_t Image::NumComponents() const {
  if (disposed_.load()) throw disposed_object_exception();
  return components_.Length();
}

const IImageComponent* Image::GetComponent(uint8_t comp_num) const {
  if (disposed_.load()) throw disposed_object_exception();
  return components_[comp_num].get();
}

const Array<IImageComponent*> Image::Components() const {
  if (disposed_.load()) throw disposed_object_exception();
  Array<IImageComponent*> components_array(components_.Length());
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
  if (disposed_.load()) throw disposed_object_exception();
  for (uint8_t c = 0; c < components_.Length(); c++) {
    if (components_[c]->IsAlpha()) return true;
  }

  return false;
}

bool Image::IsValid() const {
  if (disposed_.load()) return false;
  bool result = !(components_.IsEmpty() || width_ == 0 || height_ == 0 ||
                  color_space_ == ColorSpace::kUnsupported ||
                  chroma_subsampling_ == ChromaSubsampling::kUnsupported);
  for (uint8_t c = 0; c < components_.Length(); ++c) {
    result = result && components_[c]->IsValid();
  }

  return result;
}

std::string Image::ToString() const {
  std::ostringstream ss;
  ss << "Image:\n";
  ss << " Width: " << width_ << "\n";
  ss << " Height: " << height_ << "\n";
  ss << " ColorSpace: " << ColorSpaceToString(color_space_) << "\n";
  ss << " ChromaSubsampling: " << ChromaSubsamplingToString(chroma_subsampling_)
     << "\n";
  for (uint8_t c = 0; c < components_.Length(); ++c) {
    ss << " Component " << static_cast<int>(c) << ": "
       << components_[c]->ToString() << "\n";
  }

  return ss.str();
}

void Image::Dispose() {
  if (disposed_.exchange(true)) return;
  for (uint8_t c = 0; c < components_.Length(); ++c) {
    if (!components_[c]) continue;
    components_[c]->Dispose();
  }
}

std::ostream& operator<<(std::ostream& os, const Image& image) {
  os << image.ToString();
  return os;
}
}  // namespace imaging
}  // namespace ws
