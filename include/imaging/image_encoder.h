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

  ImageEncoder(const ImageEncoder& other);

  ImageEncoder(ImageEncoder&& other) noexcept;

  virtual ~ImageEncoder() = default;

  virtual const ImageFormat& Format() const = 0;

  virtual void Encode(const Image& image, ws::io::Stream& stream) const = 0;
  // TODO: Enable async

  ImageEncoder& operator=(const ImageEncoder& other);

  ImageEncoder& operator=(ImageEncoder&& other) noexcept;

 protected:
  std::unique_ptr<ws::logging::ILogger> logger_;
  ImageEncodingType encoding_type_;
  ImageContext context_;
  int quality_;

  ImageEncoder(const ImageContext& context, int quality,
               const std::string& source_context);

  ImageEncoder(const ImageContext& context, const std::string& source_context);
};
}  // namespace imaging
}  // namespace ws
