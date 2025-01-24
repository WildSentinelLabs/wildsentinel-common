#pragma once
#include <cstring>

#include "pixel/chroma_subsampling.h"
#include "pixel/color_space.h"
#include "pixel/pixel_format.h"
#include "pixel/pixel_format_constraints.h"
#include "pixel/pixel_plane.h"
#include "pixel/pixel_traits.h"

class ImageCore {
 public:
  ImageCore(IPixelPlane** components, const uint8_t num_components,
            const uint32_t width, const uint32_t height,
            const ColorSpace color_space,
            const ChromaSubsampling chroma_subsampling,
            const uint8_t alignment = 0);

  ~ImageCore();

  template <typename T>
  static ImageCore* LoadFromInterleavedBuffer(T* buffer, size_t size,
                                              uint32_t width, uint32_t height,
                                              uint8_t bit_depth,
                                              PixelFormat pixel_format);

  template <typename T>
  static ImageCore* LoadFromPlanarBuffer(T* buffer, size_t size, uint32_t width,
                                         uint32_t height, uint8_t bit_depth,
                                         PixelFormat pixel_format);

  uint8_t GetNumComponents() const;

  const IPixelPlane* GetComponent(uint8_t comp_num) const;

  uint32_t GetWidth() const;

  uint32_t GetHeight() const;

  const ColorSpace GetColorSpace() const;

  const ChromaSubsampling GetChromaSubsampling() const;

  uint8_t GetAlignment() const;

  bool HasAlpha() const;

  bool IsValid() const;

  template <typename T>
  T* AsInterleavedBuffer(const PixelFormat& pixel_format) const;

  template <typename T>
  T* AsPlanarBuffer(const PixelFormat& pixel_format) const;

  std::string ToString() const;

  void Dispose();

 private:
  uint8_t num_components_;
  IPixelPlane** components_;
  uint32_t width_;
  uint32_t height_;
  ColorSpace color_space_;
  ChromaSubsampling chroma_subsampling_;
  uint8_t alignment_;

  template <typename T>
  static ImageCore* InnerLoadFromInterleavedBuffer(
      T* buffer, size_t size, uint32_t width, uint32_t height,
      uint8_t bit_depth, const PixelFormatDetails* pixel_format_details);

  template <typename T>
  static ImageCore* InnerLoadFromPlanarBuffer(
      T* buffer, size_t size, uint32_t width, uint32_t height,
      uint8_t bit_depth, const PixelFormatDetails* pixel_format_details);
};

std::ostream& operator<<(std::ostream& os, const ImageCore& image_core);
