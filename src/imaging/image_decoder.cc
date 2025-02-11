#include "imaging/image_decoder.h"

ImageDecoder::ImageDecoder(const ImageContext context,
                           const std::string source_context)
    : context_(context),
      logger_(logger_configuration_.CreateLogger(source_context)) {};

LoggerConfiguration ImageDecoder::logger_configuration_ =
    LoggerConfiguration()
        .SetMinimumLogLevel(LogLevel::kInformation)
        .AddEnricher(std::make_shared<ThreadIdEnricher>())
        .AddConsoleSink(
            LogLevel::kVerbose,
            "{Timestamp:%Y-%m-%d %X.%f} [{Level:u3}] - [ThreadId: {ThreadId}] "
            "[{SourceContext}] {Message:lj}{NewLine}",
            false);
