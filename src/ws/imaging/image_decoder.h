#pragma once

#include "ws/imaging/image.h"
#include "ws/imaging/image_format.h"
#include "ws/io/stream.h"
#include "ws/logging/ilogger.h"
#include "ws/status/status_or.h"

namespace ws {
namespace imaging {
class ImageDecoder {
 public:
  ImageDecoder(const ImageDecoder&) = delete;
  ImageDecoder(ImageDecoder&&) noexcept;

  ImageDecoder& operator=(const ImageDecoder&) = delete;
  ImageDecoder& operator=(ImageDecoder&&) noexcept;

  virtual ~ImageDecoder() = default;

  void SetLogger(
      std::unique_ptr<ws::logging::ILoggerOf<ImageDecoder>>&& logger);
  virtual const ImageFormat& Format() const = 0;
  virtual StatusOr<Image> Decode(ws::io::Stream& stream) const = 0;

 protected:
  ImageDecoder(
      const ImageContext& context,
      std::unique_ptr<ws::logging::ILoggerOf<ImageDecoder>>&& logger = nullptr);

  std::unique_ptr<ws::logging::ILoggerOf<ImageDecoder>> logger_;
  ImageContext context_;
};
}  // namespace imaging
}  // namespace ws
