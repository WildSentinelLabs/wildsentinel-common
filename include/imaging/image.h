#pragma once
#include <atomic>
#include <cstring>
#include <format>

#include "array.h"
#include "idisposable.h"
#include "imaging/chroma_subsampling.h"
#include "imaging/color_space.h"
#include "imaging/image_component.h"
#include "imaging/image_context.h"

namespace ws {
namespace imaging {

class Image {
 public:
  Image();

  Image(Array<std::unique_ptr<IImageComponent>>&& components, uint32_t width,
        uint32_t height, ColorSpace color_space,
        ChromaSubsampling chroma_subsampling);

  ~Image() = default;

  Image(Image&& other) noexcept;

  Image(const Image&) = delete;

  static Image Empty();

  void LoadContext(const ImageContext& context);

  ImageContext Context() const;

  uint8_t NumComponents() const;

  const IImageComponent* GetComponent(uint8_t comp_num) const;

  Array<const IImageComponent*> Components() const;

  uint32_t Width() const;

  uint32_t Height() const;

  ColorSpace GetColorSpace() const;

  ChromaSubsampling GetChromaSubsampling() const;

  bool HasAlpha() const;

  bool IsValid() const;

  std::string ToString() const;

  Image& operator=(const Image&) = delete;

  Image& operator=(Image&& other) noexcept;

 private:
  Array<std::unique_ptr<IImageComponent>> components_;
  ImageContext context_;
  uint32_t width_;
  uint32_t height_;
  ColorSpace color_space_;
  ChromaSubsampling chroma_subsampling_;
};
}  // namespace imaging
}  // namespace ws
