#include "imaging/image_decoder.h"
namespace ws {
namespace imaging {

ImageDecoder::ImageDecoder(const ImageContext context,
                           const std::string source_context)
    : context_(context),
      logger_(logger_configuration_.CreateLogger(source_context)) {};

ws::logging::LoggerConfiguration ImageDecoder::logger_configuration_ =
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
