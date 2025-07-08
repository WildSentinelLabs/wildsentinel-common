#pragma once
#include <string>

#include "imaging/image.h"
#include "imaging/image_traits.h"
#include "logging/enrichers/thread_id_enricher.h"
#include "logging/ilogger.h"
#include "logging/logger_configuration.h"

namespace ws {
namespace imaging {

class ImageConverter {
 public:
  ImageConverter(const ImageConverter&);
  ImageConverter(ImageConverter&&) noexcept;

  ImageConverter& operator=(const ImageConverter&);
  ImageConverter& operator=(ImageConverter&&) noexcept;

  virtual ~ImageConverter() = default;

  ColorSpace GetColorSpace() const;
  ChromaSubsampling GetChromaSubsampling() const;
  virtual StatusOr<Image> Convert(const Image& source) const = 0;

  static ws::logging::LoggerConfiguration logger_configuration_;

 protected:
  ImageConverter(ColorSpace color_space, ChromaSubsampling chroma_subsampling,
                 uint8_t num_components, const std::string& source_context);

  std::unique_ptr<ws::logging::ILogger> logger_;
  std::string source_context_;
  ColorSpace color_space_;
  ChromaSubsampling chroma_subsampling_;
  uint8_t num_components_;
  uint8_t alignment_;
};

// ============================================================================
// Implementation details for ImageConverter
// ============================================================================

inline ColorSpace ImageConverter::GetColorSpace() const { return color_space_; }

inline ChromaSubsampling ImageConverter::GetChromaSubsampling() const {
  return chroma_subsampling_;
}
}  // namespace imaging
}  // namespace ws
