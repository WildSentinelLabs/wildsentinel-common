#include "imaging/image_encoder.h"
namespace ws {
namespace imaging {

ImageEncoder::ImageEncoder(const ImageContext& context,
                           const std::string& source_context,
                           const ImageCompressionOptions& compression_options)
    : context_(context),
      source_context_(source_context),
      compression_options_(compression_options),
      logger_(logger_configuration_.CreateLogger(source_context)) {}

ImageEncoder::ImageEncoder(const ImageEncoder& other)
    : context_(other.context_),
      source_context_(other.source_context_),
      compression_options_(other.compression_options_),
      logger_(logger_configuration_.CreateLogger(source_context_)) {}

ImageEncoder::ImageEncoder(ImageEncoder&& other) noexcept
    : context_(std::move(other.context_)),
      source_context_(std::move(other.source_context_)),
      compression_options_(std::move(other.compression_options_)),
      logger_(std::move(other.logger_)) {
  other.context_ = ImageContext();
}

ImageEncoder& ImageEncoder::operator=(const ImageEncoder& other) {
  if (this != &other) {
    context_ = other.context_;
    compression_options_ = other.compression_options_;
    source_context_ = other.source_context_;
    logger_ = logger_configuration_.CreateLogger(source_context_);
  }

  return *this;
}

ImageEncoder& ImageEncoder::operator=(ImageEncoder&& other) noexcept {
  if (this != &other) {
    context_ = std::move(other.context_);
    compression_options_ = std::move(other.compression_options_);
    source_context_ = std::move(other.source_context_);
    logger_ = std::move(other.logger_);
  }

  return *this;
}

void ImageEncoder::SetOptions(const ImageCompressionOptions& options) {
  compression_options_ = options;
}

ws::logging::LoggerConfiguration ImageEncoder::logger_configuration_ =
    ws::logging::LoggerConfiguration()
        .SetMinimumLogLevel(ws::logging::LogLevel::kInformation)
        .AddEnricher<ws::logging::ThreadIdEnricher>()
        .AddConsoleSink(ws::logging::LogLevel::kVerbose,
                        "{Timestamp:%Y-%m-%d %X.%f} [{Level:u3}] - "
                        "[ThreadId: {ThreadId}] "
                        "[{SourceContext}] {Message:lj}{NewLine}",
                        false);
}  // namespace imaging
}  // namespace ws
