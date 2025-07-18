#pragma once

#include "imaging/image.h"
#include "imaging/image_format.h"
#include "io/stream.h"
#include "logging/enrichers/thread_id_enricher.h"
#include "logging/ilogger.h"
#include "logging/logger_configuration.h"
#include "status/status_or.h"

namespace ws {
namespace imaging {
class ImageDecoder {
 public:
  ImageDecoder(const ImageDecoder&) = delete;
  ImageDecoder(ImageDecoder&&) noexcept;

  ImageDecoder& operator=(const ImageDecoder&) = delete;
  ImageDecoder& operator=(ImageDecoder&&) noexcept;

  virtual ~ImageDecoder() = default;

  void SetLogger(std::unique_ptr<ws::logging::ILogger>&& logger);
  virtual const ImageFormat& Format() const = 0;
  virtual StatusOr<Image> Decode(ws::io::Stream& stream) const = 0;

 protected:
  ImageDecoder(const ImageContext& context,
               std::unique_ptr<ws::logging::ILogger>&& logger = nullptr);

  std::unique_ptr<ws::logging::ILogger> logger_;
  ImageContext context_;
};
}  // namespace imaging
}  // namespace ws
