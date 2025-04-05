#pragma once
#include "idisposable.h"
#include "imaging/image.h"
#include "imaging/image_format.h"
#include "io/stream.h"
#include "logging/enrichers/thread_id_enricher.h"
#include "logging/ilogger.h"
#include "logging/logger_configuration.h"

namespace ws {
namespace imaging {
class ImageDecoder : public IDisposable {
 public:
  static ws::logging::LoggerConfiguration logger_configuration_;

  ImageDecoder(const ImageDecoder& other);

  ImageDecoder(ImageDecoder&& other) noexcept;

  virtual ~ImageDecoder() = default;

  virtual const ImageFormat& Format() const = 0;

  virtual Image Decode(ws::io::Stream& stream) const = 0;
  // TODO: Enable async

  ImageDecoder& operator=(const ImageDecoder& other);

  ImageDecoder& operator=(ImageDecoder&& other) noexcept;

 protected:
  std::unique_ptr<ws::logging::ILogger> logger_;
  std::string source_context_;
  ImageContext context_;

  ImageDecoder(const ImageContext& context, const std::string& source_context);
};
}  // namespace imaging
}  // namespace ws
