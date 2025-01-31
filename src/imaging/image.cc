#include "imaging/image.h"

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

template <typename T>
Image* Image::LoadFromInterleavedBuffer(T* buffer, size_t size, uint32_t width,
                                        uint32_t height, uint8_t bit_depth,
                                        PixelFormat pixel_format) {
  if (!buffer || size == 0 || width == 0 || height == 0) return nullptr;
  const PixelFormatDetails* pixel_format_details =
      PixelFormatConstraints::GetInterleavedFormat(pixel_format);
  if (!pixel_format_details) return nullptr;
  return InnerLoadFromInterleavedBuffer(buffer, size, width, height, bit_depth,
                                        pixel_format_details);
}

template <typename T>
Image* Image::LoadFromPlanarBuffer(T* buffer, size_t size, uint32_t width,
                                   uint32_t height, uint8_t bit_depth,
                                   PixelFormat pixel_format) {
  if (!buffer || size == 0 || width == 0 || height == 0) return nullptr;
  const PixelFormatDetails* pixel_format_details =
      PixelFormatConstraints::GetPlanarFormat(pixel_format);
  if (!pixel_format_details) return nullptr;
  return InnerLoadFromPlanarBuffer(buffer, size, width, height, bit_depth,
                                   pixel_format_details);
}

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
                  color_space_ == ColorSpace::kUNSUPPORTED ||
                  chroma_subsampling_ == ChromaSubsampling::kUNSUPPORTED);
  for (uint8_t c = 0; c < num_components_; ++c) {
    result = result && components_[c]->IsValid();
  }

  return result;
}

template <typename T>
T* Image::AsInterleavedBuffer(const PixelFormat& pixel_format) const {
  if (!IsValid()) return nullptr;
  const PixelFormatDetails* pixel_format_details =
      PixelFormatConstraints::GetInterleavedFormat(pixel_format);
  if (!pixel_format_details) return nullptr;
  if (color_space_ != pixel_format_details->color_space ||
      chroma_subsampling_ != pixel_format_details->chroma_subsampling ||
      num_components_ != pixel_format_details->num_components ||
      HasAlpha() != pixel_format_details->HasAlpha()) {
    return nullptr;
  }

  const uint8_t components_order_size =
      pixel_format_details->components_order_size;
  const uint8_t* components_order_sequence =
      pixel_format_details->components_order_sequence;
  ImageComponent* comp = nullptr;
  T* comps_buffer[num_components_] = {nullptr};
  size_t total_size = 0;
  for (uint8_t i = 0; i < components_order_size; ++i) {
    uint8_t c = components_order_sequence[i];
    if (comps_buffer[c]) {
      continue;
    } else {
      comp = components_[c];
    }

    comps_buffer[c] = (T*)comp->GetBuffer();
    total_size += comp->GetSize();
  }

  T* buffer = new T[total_size];
  size_t offset = 0;
  size_t comps_index[num_components_];
  for (size_t i = 0; i < num_components_; i++) {
    comps_index[i] = 0;
  }

  while (offset < total_size) {
    for (uint8_t i = 0; i < components_order_size; i++) {
      uint8_t c = components_order_sequence[i];
      buffer[offset] = comps_buffer[c][comps_index[c]++];
      offset++;
    }
  }

  return buffer;
};

template <typename T>
T* Image::AsPlanarBuffer(const PixelFormat& pixel_format) const {
  if (!IsValid()) return nullptr;
  const PixelFormatDetails* pixel_format_details =
      PixelFormatConstraints::GetPlanarFormat(pixel_format);
  if (!pixel_format_details) return nullptr;
  if (color_space_ != pixel_format_details->color_space ||
      chroma_subsampling_ != pixel_format_details->chroma_subsampling ||
      num_components_ != pixel_format_details->num_components ||
      HasAlpha() != pixel_format_details->HasAlpha()) {
    return nullptr;
  }

  const uint8_t components_order_size =
      pixel_format_details->components_order_size;
  const uint8_t* components_order_sequence =
      pixel_format_details->components_order_sequence;
  ImageComponent* comp = nullptr;
  size_t comps_size[num_components_];
  T* comps_buffer[num_components_] = {nullptr};
  size_t total_size = 0;
  for (uint8_t i = 0; i < components_order_size; ++i) {
    uint8_t c = components_order_sequence[i];
    if (comps_buffer[c]) {
      continue;
    } else {
      comp = components_[c];
    }

    comps_buffer[c] = (T*)comp->GetBuffer();
    comps_size[c] = comp->GetSize();
    total_size += comps_size[c];
  }

  T* buffer = new T[total_size];
  size_t offset = 0;
  for (uint8_t i = 0; i < components_order_size; ++i) {
    uint8_t c = components_order_sequence[i];
    std::memcpy(buffer + offset, comps_buffer[c], comps_size[c] * sizeof(T));
    offset += comps_size[c];
  }

  return buffer;
};

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

template <typename T>
Image* Image::InnerLoadFromInterleavedBuffer(
    T* buffer, size_t size, uint32_t width, uint32_t height, uint8_t bit_depth,
    const PixelFormatDetails* pixel_format_details) {
  uint32_t* comps_width = nullptr;
  uint32_t* comps_height = nullptr;
  uint8_t num_components = pixel_format_details->num_components;
  ChromaSubsampling chroma_subsampling =
      pixel_format_details->chroma_subsampling;
  ColorSpace color_space = pixel_format_details->color_space;
  const uint8_t components_order_size =
      pixel_format_details->components_order_size;
  const uint8_t* components_order_sequence =
      pixel_format_details->components_order_sequence;
  if (!PixelFormatConstraints::GetComponentDimensions(
          width, height, num_components, chroma_subsampling,
          pixel_format_details->HasAlpha(), comps_width, comps_height))
    return nullptr;

  size_t total_size = 0;
  int8_t alpha_index = pixel_format_details->alpha_index;
  T* comps_buffer[num_components] = {nullptr};
  size_t comp_size;
  for (size_t i = 0; i < components_order_size; i++) {
    uint8_t c = components_order_sequence[i];
    if (comps_buffer[c]) {
      continue;
    } else {
      comp_size = comps_width[c] * comps_height[c];
    }

    comps_buffer[c] = new T[comp_size];
    total_size += comp_size;
  }

  if (total_size != size) {
    delete[] comps_width;
    delete[] comps_height;
    for (size_t c = 0; c < num_components; c++) {
      delete[] comps_buffer[c];
    }

    return nullptr;
  }

  ImageComponent** components = new ImageComponent*[num_components];
  size_t offset = 0;
  size_t comps_index[num_components];
  for (size_t i = 0; i < num_components; i++) {
    comps_index[i] = 0;
  }

  while (offset < total_size) {
    for (uint8_t i = 0; i < components_order_size; i++) {
      uint8_t c = components_order_sequence[i];
      comps_buffer[c][comps_index[c]++] = buffer[offset];
      offset++;
    }
  }

  for (size_t i = 0; i < num_components; i++) {
    uint8_t c = components_order_sequence[i];
    components[c] =
        new ImageComponent(comps_buffer[c], comps_width[c], comps_height[c],
                           bit_depth, c == alpha_index);
  }

  delete[] comps_width;
  delete[] comps_height;
  return new Image(components, num_components, width, height, color_space,
                   chroma_subsampling);
}

template <typename T>
Image* Image::InnerLoadFromPlanarBuffer(
    T* buffer, size_t size, uint32_t width, uint32_t height, uint8_t bit_depth,
    const PixelFormatDetails* pixel_format_details) {
  uint32_t* comps_width = nullptr;
  uint32_t* comps_height = nullptr;
  uint8_t num_components = pixel_format_details->num_components;
  ChromaSubsampling chroma_subsampling =
      pixel_format_details->chroma_subsampling;
  ColorSpace color_space = pixel_format_details->color_space;
  const uint8_t components_order_size =
      pixel_format_details->components_order_size;
  const uint8_t* components_order_sequence =
      pixel_format_details->components_order_sequence;
  if (!PixelFormatConstraints::GetComponentDimensions(
          width, height, num_components, chroma_subsampling,
          pixel_format_details->HasAlpha(), comps_width, comps_height))
    return nullptr;
  size_t total_size = 0;
  int8_t alpha_index = pixel_format_details->alpha_index;
  for (uint8_t c = 0; c < num_components; ++c) {
    total_size += comps_width[c] * comps_height[c];
  }

  if (total_size != size) {
    delete[] comps_width;
    delete[] comps_height;
    return nullptr;
  }

  ImageComponent** components = new ImageComponent*[num_components];
  size_t offset = 0;
  uint32_t comp_width, comp_height;
  size_t comp_size;
  for (size_t i = 0; i < components_order_size; i++) {
    uint8_t c = components_order_sequence[i];
    comp_width = comps_width[c];
    comp_height = comps_height[c];
    comp_size = comp_width * comp_height;
    T* comp_buffer = new T[comp_size];
    std::memcpy(comp_buffer, buffer + offset, comp_size * sizeof(T));
    components[c] = new ImageComponent(comp_buffer, comp_width, comp_height,
                                       bit_depth, c == alpha_index);
    offset += comp_size;
  }

  delete[] comps_width;
  delete[] comps_height;
  return new Image(components, num_components, width, height, color_space,
                   chroma_subsampling);
}

template Image* Image::LoadFromInterleavedBuffer<uint8_t>(
    uint8_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t bit_depth, PixelFormat pixel_format);
template Image* Image::LoadFromInterleavedBuffer<int16_t>(
    int16_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t bit_depth, PixelFormat pixel_format);
template Image* Image::LoadFromInterleavedBuffer<uint16_t>(
    uint16_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t bit_depth, PixelFormat pixel_format);
template Image* Image::LoadFromInterleavedBuffer<int32_t>(
    int32_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t bit_depth, PixelFormat pixel_format);
template Image* Image::LoadFromInterleavedBuffer<uint32_t>(
    uint32_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t bit_depth, PixelFormat pixel_format);

template Image* Image::LoadFromPlanarBuffer<uint8_t>(
    uint8_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t bit_depth, PixelFormat pixel_format);
template Image* Image::LoadFromPlanarBuffer<int16_t>(
    int16_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t bit_depth, PixelFormat pixel_format);
template Image* Image::LoadFromPlanarBuffer<uint16_t>(
    uint16_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t bit_depth, PixelFormat pixel_format);
template Image* Image::LoadFromPlanarBuffer<int32_t>(
    int32_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t bit_depth, PixelFormat pixel_format);
template Image* Image::LoadFromPlanarBuffer<uint32_t>(
    uint32_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t bit_depth, PixelFormat pixel_format);

template uint8_t* Image::AsInterleavedBuffer<uint8_t>(
    const PixelFormat& pixel_format) const;
template int16_t* Image::AsInterleavedBuffer<int16_t>(
    const PixelFormat& pixel_format) const;
template uint16_t* Image::AsInterleavedBuffer<uint16_t>(
    const PixelFormat& pixel_format) const;
template int32_t* Image::AsInterleavedBuffer<int32_t>(
    const PixelFormat& pixel_format) const;
template uint32_t* Image::AsInterleavedBuffer<uint32_t>(
    const PixelFormat& pixel_format) const;

template uint8_t* Image::AsPlanarBuffer<uint8_t>(
    const PixelFormat& pixel_format) const;
template int16_t* Image::AsPlanarBuffer<int16_t>(
    const PixelFormat& pixel_format) const;
template uint16_t* Image::AsPlanarBuffer<uint16_t>(
    const PixelFormat& pixel_format) const;
template int32_t* Image::AsPlanarBuffer<int32_t>(
    const PixelFormat& pixel_format) const;
template uint32_t* Image::AsPlanarBuffer<uint32_t>(
    const PixelFormat& pixel_format) const;

template Image* Image::InnerLoadFromInterleavedBuffer<uint8_t>(
    uint8_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t bit_depth, const PixelFormatDetails* pixel_format_details);
template Image* Image::InnerLoadFromInterleavedBuffer<int16_t>(
    int16_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t bit_depth, const PixelFormatDetails* pixel_format_details);
template Image* Image::InnerLoadFromInterleavedBuffer<uint16_t>(
    uint16_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t bit_depth, const PixelFormatDetails* pixel_format_details);
template Image* Image::InnerLoadFromInterleavedBuffer<int32_t>(
    int32_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t bit_depth, const PixelFormatDetails* pixel_format_details);
template Image* Image::InnerLoadFromInterleavedBuffer<uint32_t>(
    uint32_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t bit_depth, const PixelFormatDetails* pixel_format_details);

template Image* Image::InnerLoadFromPlanarBuffer<uint8_t>(
    uint8_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t bit_depth, const PixelFormatDetails* pixel_format_details);
template Image* Image::InnerLoadFromPlanarBuffer<int16_t>(
    int16_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t bit_depth, const PixelFormatDetails* pixel_format_details);
template Image* Image::InnerLoadFromPlanarBuffer<uint16_t>(
    uint16_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t bit_depth, const PixelFormatDetails* pixel_format_details);
template Image* Image::InnerLoadFromPlanarBuffer<int32_t>(
    int32_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t bit_depth, const PixelFormatDetails* pixel_format_details);
template Image* Image::InnerLoadFromPlanarBuffer<uint32_t>(
    uint32_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t bit_depth, const PixelFormatDetails* pixel_format_details);

std::ostream& operator<<(std::ostream& os, const Image& image) {
  os << image.ToString();
  return os;
}
