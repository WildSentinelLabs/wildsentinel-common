#pragma once
#include "imaging/image.h"
#include "imaging/image_format.h"
#include "io/streams/stream.h"
#include "logging/enrichers/thread_id_enricher.h"
#include "logging/ilogger.h"
#include "logging/logger_configuration.h"

class ImageDecoder {
 public:
  static LoggerConfiguration logger_configuration_;

  virtual ~ImageDecoder() = default;

  virtual const ImageFormat& TargetFormat() const = 0;

  virtual Image* Decode(Stream& stream, const size_t& length) const = 0;

  virtual void Dispose() = 0;

 protected:
  std::unique_ptr<ILogger> logger_;
  ImageContext context_;

  ImageDecoder(const ImageContext context, const std::string source_context);
};
