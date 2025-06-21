#pragma once

#include "idisposable.h"
#include "imaging/image.h"
#include "imaging/image_encoding_type.h"
#include "imaging/image_format.h"
#include "io/stream.h"
#include "logging/enrichers/thread_id_enricher.h"
#include "logging/ilogger.h"
#include "logging/logger_configuration.h"
#include "status/status.h"

namespace ws {
namespace imaging {
class ImageEncoder {
 public:
  ImageEncoder(const ImageEncoder& other);
  ImageEncoder(ImageEncoder&& other) noexcept;

  ImageEncoder& operator=(const ImageEncoder& other);
  ImageEncoder& operator=(ImageEncoder&& other) noexcept;

  virtual ~ImageEncoder() = default;

  virtual const ImageFormat& Format() const = 0;
  virtual Status Encode(const Image& image, ws::io::Stream& stream) const = 0;

  static ws::logging::LoggerConfiguration logger_configuration_;

 protected:
  ImageEncoder(const ImageContext& context, int quality,
               const std::string& source_context);
  ImageEncoder(const ImageContext& context, const std::string& source_context);

  std::unique_ptr<ws::logging::ILogger> logger_;
  std::string source_context_;
  ImageEncodingType encoding_type_;
  ImageContext context_;
  int quality_;
};
}  // namespace imaging
}  // namespace ws
