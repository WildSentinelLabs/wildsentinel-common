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
  static StatusOr<Image> Create(Array<ImageComponent>&& components,
                                uint32_t width, uint32_t height,
                                ColorSpace color_space,
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
  const Array<ImageComponent>& Components() const;
  uint32_t Width() const;
  uint32_t Height() const;
  ColorSpace GetColorSpace() const;
  ChromaSubsampling GetChromaSubsampling() const;
  bool HasAlpha() const;
  bool Empty() const;
  bool IsValid() const;
  std::string ToString() const;

 private:
  Image(Array<ImageComponent>&& components, uint32_t width, uint32_t height,
        ColorSpace color_space, ChromaSubsampling chroma_subsampling);

  Array<ImageComponent> components_;
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

inline uint8_t Image::NumComponents() const { return components_.Length(); }

inline const ImageComponent& Image::GetComponent(uint8_t comp_num) const {
  return components_[comp_num];
}

inline const Array<ImageComponent>& Image::Components() const {
  return components_;
}

inline uint32_t Image::Width() const { return width_; }

inline uint32_t Image::Height() const { return height_; }

inline ColorSpace Image::GetColorSpace() const { return color_space_; }

inline ChromaSubsampling Image::GetChromaSubsampling() const {
  return chroma_subsampling_;
}

inline bool Image::Empty() const { return components_.Empty(); }

}  // namespace imaging
}  // namespace ws
