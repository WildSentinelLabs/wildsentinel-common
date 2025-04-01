#include "imaging/image_encoder.h"
namespace ws {
namespace imaging {

ImageEncoder::ImageEncoder(const ImageContext& context, int quality,
                           const std::string& source_context)
    : encoding_type_(ImageEncodingType::kLossy),
      context_(context),
      quality_(quality),
      logger_(logger_configuration_.CreateLogger(source_context)) {
  if (quality < 10 || quality > 100) {
    throw std::invalid_argument("Quality must be between 10 and 100.");
  }
}

ImageEncoder::ImageEncoder(const ImageContext& context,
                           const std::string& source_context)
    : encoding_type_(ImageEncodingType::kLossless),
      context_(context),
      quality_(0),
      logger_(logger_configuration_.CreateLogger(source_context)) {}

ws::logging::LoggerConfiguration ImageEncoder::logger_configuration_ =
    ws::logging::LoggerConfiguration()
        .SetMinimumLogLevel(ws::logging::LogLevel::kInformation)
        .AddEnricher(
            std::make_shared<ws::logging::enrichers::ThreadIdEnricher>())
        .AddConsoleSink(
            ws::logging::LogLevel::kVerbose,
            "{Timestamp:%Y-%m-%d %X.%f} [{Level:u3}] - [ThreadId: {ThreadId}] "
            "[{SourceContext}] {Message:lj}{NewLine}",
            false);
}  // namespace imaging
}  // namespace ws
