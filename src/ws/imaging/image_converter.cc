#include "ws/imaging/image_converter.h"
namespace ws {
namespace imaging {

ImageConverter::ImageConverter(
    ColorSpace color_space, ChromaSubsampling chroma_subsampling,
    uint8_t num_components,
    std::unique_ptr<ws::logging::ILoggerOf<ImageConverter>>&& logger)
    : color_space_(color_space),
      chroma_subsampling_(chroma_subsampling),
      num_components_(num_components),
      alignment_(ImageTraits::GetChromaAlignment(chroma_subsampling)),
      logger_(std::move(logger)) {};

ImageConverter::ImageConverter(ImageConverter&& other) noexcept
    : color_space_(other.color_space_),
      chroma_subsampling_(other.chroma_subsampling_),
      num_components_(other.num_components_),
      alignment_(other.alignment_),
      logger_(std::move(other.logger_)) {
  other.color_space_ = ColorSpace::kUnsupported;
  other.chroma_subsampling_ = ChromaSubsampling::kUnsupported;
  other.num_components_ = 0;
  other.alignment_ = 0;
}

ImageConverter& ImageConverter::operator=(ImageConverter&& other) noexcept {
  if (this != &other) {
    color_space_ = other.color_space_;
    chroma_subsampling_ = other.chroma_subsampling_;
    num_components_ = other.num_components_;
    alignment_ = other.alignment_;
    logger_ = std::move(other.logger_);
  }

  return *this;
}

void ImageConverter::SetLogger(
    std::unique_ptr<ws::logging::ILoggerOf<ImageConverter>>&& logger) {
  logger_ = std::move(logger);
}
}  // namespace imaging
}  // namespace ws
