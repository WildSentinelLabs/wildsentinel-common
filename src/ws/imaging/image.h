#pragma once

#include <cstring>

#include "ws/array.h"
#include "ws/imaging/chroma_subsampling.h"
#include "ws/imaging/color_space.h"
#include "ws/imaging/image_component.h"
#include "ws/imaging/image_context.h"
#include "ws/status/status_or.h"
#include "ws/string/format.h"

namespace ws {
namespace imaging {

class Image {
 public:
  using container_type = Array<ImageComponent>;

  static StatusOr<Image> Create(container_type&& components, uint32_t width,
                                uint32_t height, ColorSpace color_space,
                                ChromaSubsampling chroma_subsampling);

  Image();
  Image(Image&&) noexcept;
  Image(const Image&) = delete;
  Image& operator=(const Image&) = delete;
  Image& operator=(Image&&) noexcept;

  ~Image() = default;

  void LoadContext(const ImageContext& context);
  ImageContext Context() const;
  uint8_t NumComponents() const;
  const ImageComponent& GetComponent(uint8_t comp_num) const;
  const container_type& Components() const;
  constexpr uint32_t Width() const;
  constexpr uint32_t Height() const;
  constexpr ColorSpace GetColorSpace() const;
  constexpr ChromaSubsampling GetChromaSubsampling() const;
  bool HasAlpha() const;
  bool Empty() const;
  bool IsValid() const;
  std::string ToString() const;

 private:
  Image(container_type&& components, uint32_t width, uint32_t height,
        ColorSpace color_space, ChromaSubsampling chroma_subsampling);

  container_type components_;
  ImageContext context_;
  uint32_t width_;
  uint32_t height_;
  ColorSpace color_space_;
  ChromaSubsampling chroma_subsampling_;
};

// ============================================================================
// Implementation details for Image
// ============================================================================

inline ImageContext Image::Context() const { return context_; }

inline uint8_t Image::NumComponents() const { return components_.size(); }

inline const ImageComponent& Image::GetComponent(uint8_t comp_num) const {
  return components_[comp_num];
}

inline const Image::container_type& Image::Components() const {
  return components_;
}

inline constexpr uint32_t Image::Width() const { return width_; }

inline constexpr uint32_t Image::Height() const { return height_; }

inline constexpr ColorSpace Image::GetColorSpace() const {
  return color_space_;
}

inline constexpr ChromaSubsampling Image::GetChromaSubsampling() const {
  return chroma_subsampling_;
}

inline bool Image::Empty() const { return components_.empty(); }

}  // namespace imaging
}  // namespace ws
