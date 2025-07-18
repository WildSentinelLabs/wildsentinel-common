#include "imaging/image_decoder.h"
namespace ws {
namespace imaging {

ImageDecoder::ImageDecoder(const ImageContext& context,
                           std::unique_ptr<ws::logging::ILogger>&& logger)
    : context_(context), logger_(std::move(logger)) {};

ImageDecoder::ImageDecoder(ImageDecoder&& other) noexcept
    : context_(std::move(other.context_)), logger_(std::move(other.logger_)) {}

ImageDecoder& ImageDecoder::operator=(ImageDecoder&& other) noexcept {
  if (this != &other) {
    context_ = std::move(other.context_);
    logger_ = std::move(other.logger_);
  }

  return *this;
}
}  // namespace imaging
}  // namespace ws
