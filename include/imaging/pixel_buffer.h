#pragma once
#include <iostream>
#include <sstream>
#include <string>

#include "imaging/image_defs.h"
#include "imaging/pixel_format.h"

class IPixelBuffer {
 public:
  virtual ~IPixelBuffer() = default;
  virtual const void* GetPixelSource() const = 0;
  virtual PixelFormat GetPixelFormat() const = 0;
  virtual size_t GetSize() const = 0;
  virtual uint32_t GetWidth() const = 0;
  virtual uint32_t GetHeight() const = 0;
  virtual uint8_t GetBitDepth() const = 0;
  virtual uint8_t GetAlignment() const = 0;
  virtual bool IsPlanar() const = 0;
  virtual bool IsValid() const = 0;
  virtual IPixelBuffer* ToInterleaved() const = 0;
  virtual std::string ToString() const = 0;
  virtual void Dispose() = 0;
};

template <typename T>
class PixelBuffer : public IPixelBuffer {
 public:
  PixelBuffer(T* buffer, const size_t& size, const uint8_t& bit_depth,
              const uint32_t& width, const uint32_t& height,
              const PixelFormat& format, const uint8_t alignment = 1,
              const bool is_planar = false);

  ~PixelBuffer();

  const void* GetPixelSource() const override;

  PixelFormat GetPixelFormat() const override;

  size_t GetSize() const override;

  uint32_t GetWidth() const override;

  uint32_t GetHeight() const override;

  uint8_t GetBitDepth() const override;

  uint8_t GetAlignment() const override;

  bool IsPlanar() const override;

  bool IsValid() const override;

  IPixelBuffer* ToInterleaved() const override;

  std::string ToString() const override;

  void Dispose() override;

  const T* GetTypedPixelSource() const;

 private:
  T* pixel_source_;
  PixelFormat pixel_format_;
  size_t size_;
  uint32_t width_;
  uint32_t height_;
  uint8_t bit_depth_;
  uint8_t alignment_;
  bool is_planar_;
};

using PixelBuffer8 = PixelBuffer<uint8_t>;
using PixelBuffer16 = PixelBuffer<uint16_t>;
using PixelBuffer32 = PixelBuffer<uint32_t>;
using PixelBuffer12 = PixelBuffer<int16_t>;

std::ostream& operator<<(std::ostream& os, const IPixelBuffer& buffer);
