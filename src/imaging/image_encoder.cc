#include "imaging/image_encoder.h"

ImageEncoder::ImageEncoder(const ImageContext context, const int quality,
                           const std::string source_context)
    : encoding_type_(ImageEncodingType::kLossy),
      context_(context),
      quality_(quality),
      logger_(logger_configuration_.CreateLogger(source_context)) {
  if (quality < 10 || quality > 100) {
    throw std::invalid_argument("Quality must be between 10 and 100.");
  }
}

ImageEncoder::ImageEncoder(const ImageContext context,
                           const std::string source_context)
    : encoding_type_(ImageEncodingType::kLossless),
      context_(context),
      quality_(0),
      logger_(logger_configuration_.CreateLogger(source_context)) {}

LoggerConfiguration ImageEncoder::logger_configuration_ =
    LoggerConfiguration()
        .SetMinimumLogLevel(LogLevel::kInformation)
        .AddEnricher(std::make_shared<ThreadIdEnricher>())
        .AddConsoleSink(
            LogLevel::kVerbose,
            "{Timestamp:%Y-%m-%d %X} [{Level:u3}] - [ThreadId: {ThreadId}] "
            "[{SourceContext}] {Message:lj}{NewLine}",
            false);
