#include "imaging/image_decoder.h"
namespace ws {
namespace imaging {

ImageDecoder::ImageDecoder(const ImageContext& context,
                           const std::string& source_context)
    : context_(context),
      source_context_(source_context),
      logger_(logger_configuration_.CreateLogger(source_context)) {};

ImageDecoder::ImageDecoder(const ImageDecoder& other)
    : context_(other.context_),
      source_context_(other.source_context_),
      logger_(logger_configuration_.CreateLogger(source_context_)) {}

ImageDecoder::ImageDecoder(ImageDecoder&& other) noexcept
    : context_(std::move(other.context_)),
      source_context_(std::move(other.source_context_)),
      logger_(std::move(other.logger_)) {
  other.context_ = ImageContext();
}

ImageDecoder& ImageDecoder::operator=(const ImageDecoder& other) {
  if (this != &other) {
    context_ = other.context_;
    source_context_ = other.source_context_;
    logger_ = logger_configuration_.CreateLogger(source_context_);
  }

  return *this;
}

ImageDecoder& ImageDecoder::operator=(ImageDecoder&& other) noexcept {
  if (this != &other) {
    context_ = std::move(other.context_);
    source_context_ = std::move(other.source_context_);
    logger_ = std::move(other.logger_);

    other.context_ = ImageContext();
  }

  return *this;
}

ws::logging::LoggerConfiguration ImageDecoder::logger_configuration_ =
    ws::logging::LoggerConfiguration()
        .SetMinimumLogLevel(ws::logging::LogLevel::kInformation)
        .AddEnricher<ws::logging::ThreadIdEnricher>()
        .AddConsoleSink(
            ws::logging::LogLevel::kVerbose,
            "{Timestamp:%Y-%m-%d %X.%f} [{Level:u3}] - [ThreadId: {ThreadId}] "
            "[{SourceContext}] {Message:lj}{NewLine}",
            false);
}  // namespace imaging
}  // namespace ws
