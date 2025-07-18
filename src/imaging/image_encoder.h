#pragma once

#include "imaging/image.h"
#include "imaging/image_compression_options.h"
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
  ImageEncoder(const ImageEncoder&);
  ImageEncoder(ImageEncoder&&) noexcept;

  ImageEncoder& operator=(const ImageEncoder&);
  ImageEncoder& operator=(ImageEncoder&&) noexcept;

  virtual ~ImageEncoder() = default;

  ImageCompressionOptions Options() const;
  virtual void SetOptions(const ImageCompressionOptions& options);
  virtual const ImageFormat& Format() const = 0;
  virtual Status Encode(const Image& image, ws::io::Stream& stream) const = 0;

  static ws::logging::LoggerConfiguration logger_configuration_;

 protected:
  ImageEncoder(const ImageContext& context, const std::string& source_context,
               const ImageCompressionOptions& compression_options = {});

  std::unique_ptr<ws::logging::ILogger> logger_;
  std::string source_context_;
  ImageCompressionOptions compression_options_;
  ImageContext context_;
};

// ============================================================================
// Implementation details for ImageEncoder<T>
// ============================================================================

ImageCompressionOptions ImageEncoder::Options() const {
  return compression_options_;
}
}  // namespace imaging
}  // namespace ws
