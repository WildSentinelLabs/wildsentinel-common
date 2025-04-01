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

  virtual ~ImageDecoder() = default;

  virtual const ImageFormat& Format() const = 0;

  virtual std::unique_ptr<Image> Decode(ws::io::Stream& stream) const = 0;
  // TODO: Enable async

 protected:
  std::unique_ptr<ws::logging::ILogger> logger_;
  ImageContext context_;

  ImageDecoder(const ImageContext& context, const std::string& source_context);
};
}  // namespace imaging
}  // namespace ws
