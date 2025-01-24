#pragma once
#include <cstring>

#include "imaging/image_component.h"
#include "imaging/image_context.h"
#include "pixel/chroma_subsampling.h"
#include "pixel/color_space.h"
#include "pixel/pixel_format.h"
#include "pixel/pixel_format_constraints.h"
#include "pixel/pixel_traits.h"

class Image {
 public:
  Image(ImageComponent** components, const uint8_t num_components,
        const uint32_t width, const uint32_t height,
        const ColorSpace color_space,
        const ChromaSubsampling chroma_subsampling);

  ~Image();

  template <typename T>
  static Image* LoadFromInterleavedBuffer(T* buffer, size_t size,
                                          uint32_t width, uint32_t height,
                                          uint8_t bit_depth,
                                          PixelFormat pixel_format);

  template <typename T>
  static Image* LoadFromPlanarBuffer(T* buffer, size_t size, uint32_t width,
                                     uint32_t height, uint8_t bit_depth,
                                     PixelFormat pixel_format);

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

  template <typename T>
  T* AsInterleavedBuffer(const PixelFormat& pixel_format) const;

  template <typename T>
  T* AsPlanarBuffer(const PixelFormat& pixel_format) const;

  std::string ToString() const;

  void Dispose();

 private:
  ImageComponent** components_;
  ImageContext* context_;
  uint8_t num_components_;
  uint32_t width_;
  uint32_t height_;
  ColorSpace color_space_;
  ChromaSubsampling chroma_subsampling_;

  template <typename T>
  static Image* InnerLoadFromInterleavedBuffer(
      T* buffer, size_t size, uint32_t width, uint32_t height,
      uint8_t bit_depth, const PixelFormatDetails* pixel_format_details);

  template <typename T>
  static Image* InnerLoadFromPlanarBuffer(
      T* buffer, size_t size, uint32_t width, uint32_t height,
      uint8_t bit_depth, const PixelFormatDetails* pixel_format_details);
};

std::ostream& operator<<(std::ostream& os, const Image& image);
