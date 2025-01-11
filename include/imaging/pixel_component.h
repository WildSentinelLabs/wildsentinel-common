#pragma once

#include <cstddef>
#include <cstdint>
#include <sstream>

class IPixelComponent {
 public:
  virtual ~IPixelComponent() = default;
  virtual const void* GetBuffer() const = 0;
  virtual const uint32_t& GetWidth() const = 0;
  virtual const uint32_t& GetHeight() const = 0;
  virtual const size_t GetSize() const = 0;
  virtual const uint8_t GetBitDepth() const = 0;
  virtual const uint8_t GetDx() const = 0;
  virtual const uint8_t GetDy() const = 0;
  virtual bool IsValid() const = 0;
  virtual std::string ToString() const = 0;
  virtual void Dispose() = 0;
};

template <typename T>
class PixelComponent : public IPixelComponent {
 public:
  PixelComponent();

  PixelComponent(T* buffer, uint32_t width, uint32_t height, uint8_t bit_depth,
                 uint8_t dx, uint8_t dy);

  ~PixelComponent();

  const void* GetBuffer() const override;

  const uint32_t& GetWidth() const override;

  const uint32_t& GetHeight() const override;

  const size_t GetSize() const override;

  const uint8_t GetBitDepth() const override;

  const uint8_t GetDx() const override;

  const uint8_t GetDy() const override;

  bool IsValid() const override;

  std::string ToString() const override;

  void Dispose() override;

  const T* GetTypedBuffer() const;

 private:
  T* buffer_;
  uint32_t width_;
  uint32_t height_;
  uint8_t bit_depth_;
  uint8_t dx_;
  uint8_t dy_;
};

using PixelComponent8 = PixelComponent<uint8_t>;
using PixelComponent10 = PixelComponent<int16_t>;
using PixelComponent12 = PixelComponent<int16_t>;
using PixelComponent16 = PixelComponent<uint16_t>;
using PixelComponent32 = PixelComponent<uint32_t>;

std::ostream& operator<<(std::ostream& os,
                         const IPixelComponent& pixel_component);
