#pragma once

#include <cstddef>
#include <cstdint>
#include <sstream>

class IPixelComponent {
 public:
  virtual ~IPixelComponent() = default;
  virtual const void* GetBuffer() const = 0;
  virtual uint32_t GetWidth() const = 0;
  virtual uint32_t GetHeight() const = 0;
  virtual size_t GetSize() const = 0;
  virtual uint8_t GetBitDepth() const = 0;
  virtual bool IsAlpha() const = 0;
  virtual bool IsValid() const = 0;
  virtual std::string ToString() const = 0;
  virtual void Dispose() = 0;
};

template <typename T>
class PixelComponent : public IPixelComponent {
 public:
  PixelComponent();

  PixelComponent(T* buffer, uint32_t width, uint32_t height, uint8_t bit_depth,
                 bool is_alpha = false);

  ~PixelComponent();

  const void* GetBuffer() const override;

  uint32_t GetWidth() const override;

  uint32_t GetHeight() const override;

  size_t GetSize() const override;

  uint8_t GetBitDepth() const override;

  bool IsAlpha() const override;

  bool IsValid() const override;

  std::string ToString() const override;

  void Dispose() override;

  const T* GetTypedBuffer() const;

 private:
  T* buffer_;
  uint32_t width_;
  uint32_t height_;
  uint8_t bit_depth_;
  bool is_alpha_;
};

using PixelComponent8 = PixelComponent<uint8_t>;
using PixelComponent12 = PixelComponent<int16_t>;
using PixelComponent16 = PixelComponent<uint16_t>;
using PixelComponent24 = PixelComponent<int32_t>;
using PixelComponent32 = PixelComponent<uint32_t>;

std::ostream& operator<<(std::ostream& os,
                         const IPixelComponent& pixel_component);
