#pragma once
#include <string>

#include "imaging/image.h"
#include "logging/enrichers/thread_id_enricher.h"
#include "logging/ilogger.h"
#include "logging/logger_configuration.h"
namespace ws {
namespace imaging {

class ImageConverter {
 public:
  static ws::logging::LoggerConfiguration logger_configuration_;

  virtual ~ImageConverter() = default;

  virtual Image* Convert(const Image& source) const = 0;

  const ColorSpace GetColorSpace() const;

  const ChromaSubsampling GetChromaSubsampling() const;

 protected:
  std::unique_ptr<ws::logging::ILogger> logger_;
  ColorSpace color_space_;
  ChromaSubsampling chroma_subsampling_;
  uint8_t num_components_;
  uint8_t alignment_;

  ImageConverter(const std::string source_context, const ColorSpace color_space,
                 const ChromaSubsampling chroma_subsampling,
                 const uint8_t num_components);
};
}  // namespace imaging
}  // namespace ws
