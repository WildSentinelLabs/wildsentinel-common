#pragma once
#include <cstring>

#include "imaging/image_defs.h"
#include "imaging/pixel_align.h"
#include "imaging/pixel_component.h"

class ImageCore {
 public:
  ImageCore(IPixelComponent** components, const uint8_t num_components,
            const uint32_t width, const uint32_t height,
            const uint8_t bit_depth, const ColorSpace color_space,
            const ChromaSubsampling subsampling, const uint8_t alignment = 0);

  ~ImageCore();

  template <typename T>
  static ImageCore* LoadFromInterleavedBuffer(T* buffer, size_t size,
                                              uint32_t width, uint32_t height,
                                              uint8_t num_comps,
                                              uint8_t bit_depth,
                                              ColorSpace color_space,
                                              ChromaSubsampling subsamp);

  template <typename T>
  static ImageCore* LoadFromPlanarBuffer(T* buffer, size_t size, uint32_t width,
                                         uint32_t height, uint8_t num_comps,
                                         uint8_t bit_depth,
                                         ColorSpace color_space,
                                         ChromaSubsampling subsamp);

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
  T* AsInterleavedBuffer() const;

  template <typename T>
  T* AsPlanarBuffer() const;

  std::string ToString() const;

  void Dispose();

 private:
  uint8_t num_components_;
  IPixelComponent** components_;
  uint32_t width_;
  uint32_t height_;
  uint8_t bit_depth_;
  ColorSpace color_space_;
  ChromaSubsampling subsampling_;
  uint8_t alignment_;

  static bool GetComponentDimensions(uint32_t width, uint32_t height,
                                     uint8_t num_comps,
                                     ChromaSubsampling subsamp,
                                     uint32_t*& comps_width,
                                     uint32_t*& comps_height,
                                     uint8_t*& comps_dx, uint8_t*& comps_dy);
};

std::ostream& operator<<(std::ostream& os, const ImageCore& image_core);
