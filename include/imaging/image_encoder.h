#pragma once
#include "imaging/image.h"
#include "imaging/image_encoding_type.h"
#include "imaging/image_format.h"
#include "io/streams/stream.h"
#include "logging/enrichers/thread_id_enricher.h"
#include "logging/ilogger.h"
#include "logging/logger_configuration.h"

class ImageEncoder {
 public:
  static LoggerConfiguration logger_configuration_;

  virtual ~ImageEncoder() = default;

  virtual const ImageFormat& Format() const = 0;

  virtual bool Encode(Stream& stream, const Image& image) const = 0;

  virtual void Dispose() = 0;

 protected:
  std::unique_ptr<ILogger> logger_;
  ImageEncodingType encoding_type_;
  ImageContext context_;
  int quality_;

  ImageEncoder(const ImageContext context, const int quality,
               const std::string source_context);

  ImageEncoder(const ImageContext context, const std::string source_context);
};
