#pragma once
#include <atomic>
#include <cstring>
#include <vector>

#include "idisposable.h"
#include "imaging/chroma_subsampling.h"
#include "imaging/color_space.h"
#include "imaging/image_component.h"
#include "imaging/image_context.h"

namespace ws {
namespace imaging {

class Image : public IDisposable {
 public:
  Image(ImageComponent** components, const uint8_t num_components,
        const uint32_t width, const uint32_t height,
        const ColorSpace color_space,
        const ChromaSubsampling chroma_subsampling);

  ~Image();

  void LoadContext(ImageContext context) const;

  const ImageContext* GetContext() const;

  uint8_t GetNumComponents() const;

  const ImageComponent* GetComponent(uint8_t comp_num) const;

  uint32_t GetWidth() const;

  uint32_t GetHeight() const;

  const ColorSpace GetColorSpace() const;

  const ChromaSubsampling GetChromaSubsampling() const;

  bool HasAlpha() const;

  bool IsValid() const;

  std::string ToString() const;

  void Dispose() override;

 private:
  ImageComponent** components_;
  ImageContext* context_;
  std::uint8_t num_components_;
  uint32_t width_;
  uint32_t height_;
  ColorSpace color_space_;
  ChromaSubsampling chroma_subsampling_;
  std::atomic<bool> disposed_;
};

std::ostream& operator<<(std::ostream& os, const Image& image);

// TODO: Enhace image::Load and image::AsArray
// TODO: Enable async operations
}  // namespace imaging
}  // namespace ws
