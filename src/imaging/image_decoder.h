#pragma once
#include "idisposable.h"
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
  ImageDecoder(const ImageDecoder& other);
  ImageDecoder(ImageDecoder&& other) noexcept;

  ImageDecoder& operator=(const ImageDecoder& other);
  ImageDecoder& operator=(ImageDecoder&& other) noexcept;

  virtual ~ImageDecoder() = default;

  virtual const ImageFormat& Format() const = 0;
  virtual StatusOr<Image> Decode(ws::io::Stream& stream) const = 0;

  static ws::logging::LoggerConfiguration logger_configuration_;

 protected:
  ImageDecoder(const ImageContext& context, const std::string& source_context);

  std::unique_ptr<ws::logging::ILogger> logger_;
  std::string source_context_;
  ImageContext context_;
};
}  // namespace imaging
}  // namespace ws
