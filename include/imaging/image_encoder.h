#pragma once
#include "idisposable.h"
#include "imaging/image.h"
#include "imaging/image_encoding_type.h"
#include "imaging/image_format.h"
#include "io/stream.h"
#include "logging/enrichers/thread_id_enricher.h"
#include "logging/ilogger.h"
#include "logging/logger_configuration.h"

namespace ws {
namespace imaging {

class ImageEncoder : public IDisposable {
 public:
  static ws::logging::LoggerConfiguration logger_configuration_;

  virtual ~ImageEncoder() = default;

  virtual const ImageFormat& Format() const = 0;

  virtual bool Encode(ws::io::Stream& stream, const Image& image) const = 0;
  // TODO: Enable async

 protected:
  std::unique_ptr<ws::logging::ILogger> logger_;
  ImageEncodingType encoding_type_;
  ImageContext context_;
  int quality_;

  ImageEncoder(const ImageContext context, const int quality,
               const std::string source_context);

  ImageEncoder(const ImageContext context, const std::string source_context);
};
}  // namespace imaging
}  // namespace ws
