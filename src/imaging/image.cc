#include "imaging/image.h"
namespace ws {
namespace imaging {

Image::Image(ImageComponent** components, const uint8_t num_components,
             const uint32_t width, const uint32_t height,
             const ColorSpace color_space,
             const ChromaSubsampling chroma_subsampling)
    : components_(components),
      num_components_(num_components),
      width_(width),
      height_(height),
      color_space_(color_space),
      chroma_subsampling_(chroma_subsampling) {
  if (!IsValid())
    throw std::invalid_argument(
        "Invalid parameters: buffers, dimensions, and bit depth must be "
        "non-null and non-zero");

  context_ = new ImageContext();
}

Image::~Image() { Dispose(); }

void Image::LoadContext(ImageContext context) const {
  context_->Clean();
  for (const auto& [key, value] : context) {
    context_->Add(key, value);
  }
}

const ImageContext* Image::GetContext() const { return context_; }

uint8_t Image::GetNumComponents() const { return num_components_; }

const ImageComponent* Image::GetComponent(uint8_t comp_num) const {
  if (comp_num >= num_components_ || comp_num < 0) return nullptr;
  return components_[comp_num];
}

uint32_t Image::GetWidth() const { return width_; }

uint32_t Image::GetHeight() const { return height_; }

const ColorSpace Image::GetColorSpace() const { return color_space_; }

const ChromaSubsampling Image::GetChromaSubsampling() const {
  return chroma_subsampling_;
}

bool Image::HasAlpha() const {
  for (uint8_t c = 0; c < num_components_; c++) {
    if (components_[c]->IsAlpha()) return true;
  }

  return false;
}

bool Image::IsValid() const {
  bool result = !(components_ == nullptr || width_ == 0 || height_ == 0 ||
                  color_space_ == ColorSpace::kUnsupported ||
                  chroma_subsampling_ == ChromaSubsampling::kUnsupported);
  for (uint8_t c = 0; c < num_components_; ++c) {
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
  for (uint8_t c = 0; c < num_components_; ++c) {
    ss << " Component " << static_cast<int>(c) << ": "
       << components_[c]->ToString() << "\n";
  }

  return ss.str();
}

void Image::Dispose() {
  if (components_) {
    for (uint8_t c = 0; c < num_components_; ++c) {
      if (!components_[c]) continue;
      components_[c]->Dispose();
      delete components_[c];
      components_[c] = nullptr;
    }

    delete[] components_;
    components_ = nullptr;
  }

  if (context_) {
    delete context_;
    context_ = nullptr;
  }
}

std::ostream& operator<<(std::ostream& os, const Image& image) {
  os << image.ToString();
  return os;
}
}  // namespace imaging
}  // namespace ws
