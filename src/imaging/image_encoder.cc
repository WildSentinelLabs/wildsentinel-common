#include "imaging/image_encoder.h"
namespace ws {
namespace imaging {

ImageEncoder::ImageEncoder(const ImageContext& context, int quality,
                           const std::string& source_context)
    : context_(context),
      source_context_(source_context),
      encoding_type_(ImageEncodingType::kLossy),
      quality_(quality),
      logger_(logger_configuration_.CreateLogger(source_context)) {}

ImageEncoder::ImageEncoder(const ImageContext& context,
                           const std::string& source_context)
    : context_(context),
      source_context_(source_context),
      encoding_type_(ImageEncodingType::kLossless),
      quality_(0),
      logger_(logger_configuration_.CreateLogger(source_context)) {}

ImageEncoder::ImageEncoder(const ImageEncoder& other)
    : context_(other.context_),
      source_context_(other.source_context_),
      encoding_type_(other.encoding_type_),
      quality_(other.quality_),
      logger_(logger_configuration_.CreateLogger(source_context_)) {}

ImageEncoder::ImageEncoder(ImageEncoder&& other) noexcept
    : context_(std::move(other.context_)),
      source_context_(other.source_context_),
      encoding_type_(other.encoding_type_),
      quality_(other.quality_),
      logger_(std::move(other.logger_)) {
  other.context_ = ImageContext();
  other.quality_ = -1;
}

ImageEncoder& ImageEncoder::operator=(const ImageEncoder& other) {
  if (this != &other) {
    context_ = other.context_;
    encoding_type_ = other.encoding_type_;
    quality_ = other.quality_;
    source_context_ = other.source_context_;
    logger_ = logger_configuration_.CreateLogger(source_context_);
  }

  return *this;
}

ImageEncoder& ImageEncoder::operator=(ImageEncoder&& other) noexcept {
  if (this != &other) {
    context_ = std::move(other.context_);
    encoding_type_ = other.encoding_type_;
    quality_ = other.quality_;
    source_context_ = other.source_context_;
    logger_ = std::move(other.logger_);

    other.context_ = ImageContext();
    other.quality_ = -1;
  }

  return *this;
}

ws::logging::LoggerConfiguration ImageEncoder::logger_configuration_ =
    ws::logging::LoggerConfiguration()
        .SetMinimumLogLevel(ws::logging::LogLevel::kInformation)
        .AddEnricher<ws::logging::enrichers::ThreadIdEnricher>()
        .AddConsoleSink(ws::logging::LogLevel::kVerbose,
                        "{Timestamp:%Y-%m-%d %X.%f} [{Level:u3}] - "
                        "[ThreadId: {ThreadId}] "
                        "[{SourceContext}] {Message:lj}{NewLine}",
                        false);
}  // namespace imaging
}  // namespace ws
