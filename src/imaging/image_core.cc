#include "imaging/image_core.h"

#include <iostream>

ImageCore::ImageCore(IPixelComponent** components, const uint8_t num_components,
                     const uint32_t width, const uint32_t height,
                     const uint8_t bit_depth, const ColorSpace color_space,
                     const ChromaSubsampling subsampling,
                     const uint8_t alignment)
    : components_(components),
      num_components_(num_components),
      width_(width),
      height_(height),
      bit_depth_(bit_depth),
      color_space_(color_space),
      subsampling_(subsampling) {
  if (!IsValid())
    throw std::invalid_argument(
        "Invalid parameters: buffers, dimensions, and bit depth must be "
        "non-null and non-zero");
  if (alignment == 0) {
    alignment_ = PixelAlign::GetBitAlignment(bit_depth) *
                 PixelAlign::GetChromaAlignment(subsampling);
  } else {
    alignment_ = alignment;
  }
}

ImageCore::~ImageCore() { Dispose(); }

template <typename T>
ImageCore* ImageCore::LoadFromInterleavedBuffer(
    T* buffer, size_t size, uint32_t width, uint32_t height, uint8_t num_comps,
    uint8_t bit_depth, ColorSpace color_space, ChromaSubsampling subsamp) {
  if (!buffer || size == 0 || num_comps == 0) return nullptr;
  uint32_t* comps_width = nullptr;
  uint32_t* comps_height = nullptr;
  uint8_t* comps_dx = nullptr;
  uint8_t* comps_dy = nullptr;
  if (!GetComponentDimensions(width, height, num_comps, subsamp, comps_width,
                              comps_height, comps_dx, comps_dy))
    return nullptr;
  size_t total_size = 0;
  for (uint8_t c = 0; c < num_comps; ++c) {
    total_size += comps_width[c] * comps_height[c];
  }

  if (total_size != size) {
    delete[] comps_width;
    delete[] comps_height;
    delete[] comps_dx;
    delete[] comps_dy;
    return nullptr;
  }

  IPixelComponent** components = new IPixelComponent*[num_comps];
  uint32_t comp_width;
  uint32_t comp_height;
  uint8_t comp_dx;
  uint8_t comp_dy;
  size_t comp_size;
  uint32_t original_x;
  uint32_t original_y;
  size_t original_index;
  for (uint8_t c = 0; c < num_comps; ++c) {
    comp_width = comps_width[c];
    comp_height = comps_height[c];
    comp_dx = comps_dx[c];
    comp_dy = comps_dy[c];
    comp_size = comp_width * comp_height;
    T* comp_buffer = new T[comp_size];
    for (uint32_t y = 0; y < comp_height; y++) {
      for (uint32_t x = 0; x < comp_width; x++) {
        original_x = x * comp_dx;
        original_y = y * comp_dy;
        original_index = (original_y * width + original_x) * num_comps + c;
        comp_buffer[y * comp_width + x] = buffer[original_index];
      }
    }

    components[c] = new PixelComponent<T>(comp_buffer, comp_width, comp_height,
                                          bit_depth, comp_dx, comp_dy);
  }

  return new ImageCore(components, num_comps, width, height, bit_depth,
                       color_space, subsamp);
}

template <typename T>
ImageCore* ImageCore::LoadFromPlanarBuffer(T* buffer, size_t size,
                                           uint32_t width, uint32_t height,
                                           uint8_t num_comps, uint8_t bit_depth,
                                           ColorSpace color_space,
                                           ChromaSubsampling subsamp) {
  if (!buffer || size == 0 || num_comps == 0) return nullptr;
  uint32_t* comps_width = nullptr;
  uint32_t* comps_height = nullptr;
  uint8_t* comps_dx = nullptr;
  uint8_t* comps_dy = nullptr;
  if (!GetComponentDimensions(width, height, num_comps, subsamp, comps_width,
                              comps_height, comps_dx, comps_dy))
    return nullptr;
  size_t total_size = 0;
  for (uint8_t c = 0; c < num_comps; ++c) {
    total_size += comps_width[c] * comps_height[c];
  }

  if (total_size != size) {
    delete[] comps_width;
    delete[] comps_height;
    delete[] comps_dx;
    delete[] comps_dy;
    return nullptr;
  }

  IPixelComponent** components = new IPixelComponent*[num_comps];
  size_t offset = 0;
  uint32_t comp_width = 0;
  uint32_t comp_height = 0;
  uint8_t comp_dx = 0;
  uint8_t comp_dy = 0;
  size_t comp_size = 0;
  for (uint8_t c = 0; c < num_comps; ++c) {
    comp_width = comps_width[c];
    comp_height = comps_height[c];
    comp_dx = comps_dx[c];
    comp_dy = comps_dy[c];
    comp_size = comp_width * comp_height;
    T* comp_buffer = new T[comp_size];
    std::memcpy(comp_buffer, buffer + offset, comp_size * sizeof(T));
    components[c] = new PixelComponent<T>(comp_buffer, comp_width, comp_height,
                                          bit_depth, comp_dx, comp_dy);
    offset += comp_size;
  }

  return new ImageCore(components, num_comps, width, height, bit_depth,
                       color_space, subsamp);
}

const uint8_t ImageCore::GetNumComponents() const { return num_components_; }

const IPixelComponent* ImageCore::GetComponent(uint8_t comp_num) const {
  if (comp_num >= num_components_ || comp_num < 0) return nullptr;
  return components_[comp_num];
}

const uint32_t& ImageCore::GetWidth() const { return width_; }

const uint32_t& ImageCore::GetHeight() const { return height_; }

const uint8_t ImageCore::GetBitDepth() const { return bit_depth_; }

const ColorSpace& ImageCore::GetColorSpace() const { return color_space_; }

const ChromaSubsampling& ImageCore::GetChromaSubsampling() const {
  return subsampling_;
}

const uint8_t ImageCore::GetAlignment() const { return alignment_; }

const bool ImageCore::IsValid() const {
  bool result = !(components_ == nullptr || width_ == 0 || height_ == 0);
  for (uint8_t c = 0; c < num_components_; ++c) {
    result = result && components_[c]->IsValid();
  }

  return result;
}

template <typename T>
T* ImageCore::AsInterleavedBuffer() const {
  if (!IsValid()) return nullptr;
  size_t total_size = 0;
  IPixelComponent* comp = nullptr;
  for (uint8_t c = 0; c < num_components_; ++c) {
    comp = components_[c];
    total_size += comp->GetSize();
  }

  T* buffer = new T[total_size];
  size_t offset = 0;
  uint32_t width = components_[0]->GetWidth();
  uint32_t height = components_[0]->GetHeight();
  uint32_t comp_width;
  uint32_t comp_height;
  uint8_t comp_dx;
  uint8_t comp_dy;
  size_t comp_size;
  uint32_t original_x;
  uint32_t original_y;
  size_t comp_index;
  for (uint8_t c = 0; c < num_components_; ++c) {
    comp = components_[c];
    comp_width = comp->GetWidth();
    comp_height = comp->GetHeight();
    comp_dx = comp->GetDx();
    comp_dy = comp->GetDy();
    const T* comp_buffer = (T*)comp->GetBuffer();
    for (uint32_t y = 0; y < comp_height; y++) {
      for (uint32_t x = 0; x < comp_width; x++) {
        original_x = x * comp_dx;
        original_y = y * comp_dy;
        if (original_y == 1532) {
          std::cout << "original_y: " << (size_t)original_y;
        }
        comp_index = original_y * comp_width + original_x;
        // << "original_x: " << original_x
        buffer[offset++] = comp_buffer[comp_index];
      }
    }
    offset += comp_size;
  }

  return buffer;
};

template <typename T>
T* ImageCore::AsPlanarBuffer() const {
  if (!IsValid()) return nullptr;
  size_t total_size = 0;
  IPixelComponent* comp = nullptr;
  for (uint8_t c = 0; c < num_components_; ++c) {
    comp = components_[c];
    total_size += comp->GetSize();
  }

  T* buffer = new T[total_size];
  size_t offset = 0;
  uint32_t comp_width;
  uint32_t comp_height;
  size_t comp_size;
  for (uint8_t c = 0; c < num_components_; ++c) {
    comp = components_[c];
    comp_width = comp->GetWidth();
    comp_height = comp->GetHeight();
    comp_size = comp_width * comp_height;
    T* comp_buffer = (T*)comp->GetBuffer();
    std::memcpy(buffer + offset, comp_buffer, comp_size * sizeof(T));
    offset += comp_size;
  }

  return buffer;
};

std::string ImageCore::ToString() const {
  std::ostringstream ss;
  ss << "ImageCore:\n";
  ss << " Width: " << width_ << "\n";
  ss << " Height: " << height_ << "\n";
  ss << " ColorSpace: " << ColorSpaceToString(color_space_) << "\n";
  ss << " ChromaSubsampling: " << ChromaSubsamplingToString(subsampling_)
     << "\n";
  ss << " Alignment: " << static_cast<int>(alignment_) << "\n";
  for (uint8_t c = 0; c < num_components_; ++c) {
    ss << " Component " << static_cast<int>(c) << ": "
       << components_[c]->ToString() << "\n";
  }

  return ss.str();
}

void ImageCore::Dispose() {
  if (components_) {
    for (uint8_t c = 0; c < num_components_; ++c) {
      components_[c]->Dispose();
      delete components_[c];
    }

    delete[] components_;
    components_ = nullptr;
  }
}

bool ImageCore::GetComponentDimensions(uint32_t width, uint32_t height,
                                       uint8_t num_comps,
                                       ChromaSubsampling subsamp,
                                       uint32_t*& comps_width,
                                       uint32_t*& comps_height,
                                       uint8_t*& comps_dx, uint8_t*& comps_dy) {
  comps_width = new uint32_t[num_comps];
  comps_height = new uint32_t[num_comps];
  comps_dx = new uint8_t[num_comps];
  comps_dy = new uint8_t[num_comps];
  switch (subsamp) {
    case ChromaSubsampling::kSAMP_444:
      for (uint8_t c = 0; c < num_comps; ++c) {
        comps_width[c] = width;
        comps_height[c] = height;
        comps_dx[c] = 1;
        comps_dy[c] = 1;
      }

      return true;
      break;

    case ChromaSubsampling::kSAMP_422:
      if (num_comps != 3) break;
      comps_width[0] = width;
      comps_height[0] = height;
      comps_dx[0] = 1;
      comps_dy[0] = 1;

      comps_width[1] = comps_width[2] = width / 2;
      comps_height[1] = comps_height[2] = height;
      comps_dx[1] = comps_dx[2] = 2;
      comps_dy[1] = comps_dy[2] = 1;
      return true;
      break;

    case ChromaSubsampling::kSAMP_420:
      if (num_comps != 3) break;
      comps_width[0] = width;
      comps_height[0] = height;
      comps_dx[0] = 1;
      comps_dy[0] = 1;

      comps_width[1] = comps_width[2] = width / 2;
      comps_height[1] = comps_height[2] = height / 2;
      comps_dx[1] = comps_dx[2] = 2;
      comps_dy[1] = comps_dy[2] = 2;
      return true;
      break;

    case ChromaSubsampling::kSAMP_400:
      if (num_comps != 1) break;
      comps_width[0] = width;
      comps_height[0] = height;
      comps_dx[0] = 1;
      comps_dy[0] = 1;
      return true;
      break;

    case ChromaSubsampling::kSAMP_440:
      if (num_comps != 3) break;
      comps_width[0] = width;
      comps_height[0] = height;
      comps_dx[0] = 1;
      comps_dy[0] = 1;

      comps_width[1] = comps_width[2] = width;
      comps_height[1] = comps_height[2] = height / 2;
      comps_dx[1] = comps_dx[2] = 1;
      comps_dy[1] = comps_dy[2] = 2;
      return true;
      break;

    case ChromaSubsampling::kSAMP_411:
      if (num_comps != 3) break;
      comps_width[0] = width;
      comps_height[0] = height;
      comps_dx[0] = 1;
      comps_dy[0] = 1;

      comps_width[1] = comps_width[2] = width / 4;
      comps_height[1] = comps_height[2] = height;
      comps_dx[1] = comps_dx[2] = 4;
      comps_dy[1] = comps_dy[2] = 1;
      return true;
      break;

    default:
      delete[] comps_width;
      delete[] comps_height;
      delete[] comps_dx;
      delete[] comps_dy;
      throw std::invalid_argument("Unsupported chroma subsampling.");
  }

  delete[] comps_width;
  delete[] comps_height;
  delete[] comps_dx;
  delete[] comps_dy;
  return false;
}

template uint8_t* ImageCore::AsInterleavedBuffer<uint8_t>() const;
template int16_t* ImageCore::AsInterleavedBuffer<int16_t>() const;
template uint16_t* ImageCore::AsInterleavedBuffer<uint16_t>() const;
template int32_t* ImageCore::AsInterleavedBuffer<int32_t>() const;
template uint32_t* ImageCore::AsInterleavedBuffer<uint32_t>() const;

template uint8_t* ImageCore::AsPlanarBuffer<uint8_t>() const;
template int16_t* ImageCore::AsPlanarBuffer<int16_t>() const;
template uint16_t* ImageCore::AsPlanarBuffer<uint16_t>() const;
template int32_t* ImageCore::AsPlanarBuffer<int32_t>() const;
template uint32_t* ImageCore::AsPlanarBuffer<uint32_t>() const;

template ImageCore* ImageCore::LoadFromInterleavedBuffer<uint8_t>(
    uint8_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t num_comps, uint8_t bit_depth, ColorSpace color_space,
    ChromaSubsampling subsamp);
template ImageCore* ImageCore::LoadFromInterleavedBuffer<int16_t>(
    int16_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t num_comps, uint8_t bit_depth, ColorSpace color_space,
    ChromaSubsampling subsamp);
template ImageCore* ImageCore::LoadFromInterleavedBuffer<uint16_t>(
    uint16_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t num_comps, uint8_t bit_depth, ColorSpace color_space,
    ChromaSubsampling subsamp);
template ImageCore* ImageCore::LoadFromInterleavedBuffer<int32_t>(
    int32_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t num_comps, uint8_t bit_depth, ColorSpace color_space,
    ChromaSubsampling subsamp);
template ImageCore* ImageCore::LoadFromInterleavedBuffer<uint32_t>(
    uint32_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t num_comps, uint8_t bit_depth, ColorSpace color_space,
    ChromaSubsampling subsamp);

template ImageCore* ImageCore::LoadFromPlanarBuffer<uint8_t>(
    uint8_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t num_comps, uint8_t bit_depth, ColorSpace color_space,
    ChromaSubsampling subsamp);
template ImageCore* ImageCore::LoadFromPlanarBuffer<int16_t>(
    int16_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t num_comps, uint8_t bit_depth, ColorSpace color_space,
    ChromaSubsampling subsamp);
template ImageCore* ImageCore::LoadFromPlanarBuffer<uint16_t>(
    uint16_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t num_comps, uint8_t bit_depth, ColorSpace color_space,
    ChromaSubsampling subsamp);
template ImageCore* ImageCore::LoadFromPlanarBuffer<int32_t>(
    int32_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t num_comps, uint8_t bit_depth, ColorSpace color_space,
    ChromaSubsampling subsamp);
template ImageCore* ImageCore::LoadFromPlanarBuffer<uint32_t>(
    uint32_t* buffer, size_t size, uint32_t width, uint32_t height,
    uint8_t num_comps, uint8_t bit_depth, ColorSpace color_space,
    ChromaSubsampling subsamp);

std::ostream& operator<<(std::ostream& os, const ImageCore& image_core) {
  os << image_core.ToString();
  return os;
}
