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

  ImageConverter(const ImageConverter& other);

  ImageConverter(ImageConverter&& other) noexcept;

  virtual ~ImageConverter() = default;

  virtual Image Convert(const Image& source) const = 0;

  ColorSpace GetColorSpace() const;

  ChromaSubsampling GetChromaSubsampling() const;

  ImageConverter& operator=(const ImageConverter& other);

  ImageConverter& operator=(ImageConverter&& other) noexcept;

 protected:
  std::unique_ptr<ws::logging::ILogger> logger_;
  ColorSpace color_space_;
  ChromaSubsampling chroma_subsampling_;
  uint8_t num_components_;
  uint8_t alignment_;

  ImageConverter(ColorSpace color_space, ChromaSubsampling chroma_subsampling,
                 uint8_t num_components, std::string& source_context);
};
}  // namespace imaging
}  // namespace ws
