#include "imaging/image_encoder.h"
namespace ws {
namespace imaging {

ImageEncoder::ImageEncoder(const ImageContext& context,
                           const ImageCompressionOptions& compression_options,
                           std::unique_ptr<ws::logging::ILogger>&& logger)
    : context_(context),
      compression_options_(compression_options),
      logger_(std::move(logger)) {}

ImageEncoder::ImageEncoder(ImageEncoder&& other) noexcept
    : context_(std::move(other.context_)),
      compression_options_(std::move(other.compression_options_)),
      logger_(std::move(other.logger_)) {
  other.context_ = ImageContext();
}

ImageEncoder& ImageEncoder::operator=(ImageEncoder&& other) noexcept {
  if (this != &other) {
    context_ = std::move(other.context_);
    compression_options_ = std::move(other.compression_options_);
    logger_ = std::move(other.logger_);
  }

  return *this;
}

void ImageEncoder::SetOptions(const ImageCompressionOptions& options) {
  compression_options_ = options;
}

void ImageEncoder::SetLogger(std::unique_ptr<ws::logging::ILogger>&& logger) {
  logger_ = std::move(logger);
}
}  // namespace imaging
}  // namespace ws
