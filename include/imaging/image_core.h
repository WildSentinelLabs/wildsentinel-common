#pragma once
#include <cstring>

#include "imaging/image_defs.h"
#include "imaging/pixel_align.h"
#include "imaging/pixel_component.h"
#include "imaging/pixel_format_constraints.h"

class ImageCore {
 public:
  ImageCore(IPixelComponent** components, const uint8_t num_components,
            const uint32_t width, const uint32_t height,
            const uint8_t bit_depth, const ColorSpace color_space,
            const ChromaSubsampling chroma_subsampling,
            IPixelComponent* alpha = nullptr, const uint8_t alignment = 0);

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

  const uint8_t GetNumComponents() const;

  const IPixelComponent* GetComponent(uint8_t comp_num) const;

  const uint32_t& GetWidth() const;

  const uint32_t& GetHeight() const;

  const uint8_t GetBitDepth() const;

  const ColorSpace& GetColorSpace() const;

  const ChromaSubsampling& GetChromaSubsampling() const;

  const uint8_t GetAlignment() const;

  const bool IsValid() const;

  template <typename T>
  T* AsInterleavedBuffer(const PixelFormat& pixel_format) const;

  template <typename T>
  T* AsPlanarBuffer(const PixelFormat& pixel_format) const;

  std::string ToString() const;

  void Dispose();

 private:
  uint8_t num_components_;
  IPixelComponent** components_;
  IPixelComponent* alpha_;
  uint32_t width_;
  uint32_t height_;
  uint8_t bit_depth_;
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
