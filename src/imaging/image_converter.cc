#include "imaging/image_converter.h"

ImageConverter::ImageConverter(const std::string source_context,
                               const ColorSpace color_space,
                               const ChromaSubsampling chroma_subsampling,
                               const uint8_t num_components)
    : logger_(logger_configuration_.CreateLogger(source_context)),
      color_space_(color_space),
      chroma_subsampling_(chroma_subsampling),
      num_components_(num_components),
      alignment_(ImageTraits::GetChromaAlignment(chroma_subsampling)) {};

const ColorSpace ImageConverter::GetColorSpace() const { return color_space_; }

const ChromaSubsampling ImageConverter::GetChromaSubsampling() const {
  return chroma_subsampling_;
}

LoggerConfiguration ImageConverter::logger_configuration_ =
    LoggerConfiguration()
        .SetMinimumLogLevel(LogLevel::kInformation)
        .AddEnricher(std::make_shared<ThreadIdEnricher>())
        .AddConsoleSink(
            LogLevel::kVerbose,
            "{Timestamp:%Y-%m-%d %X} [{Level:u3}] - [ThreadId: {ThreadId}] "
            "[{SourceContext}] {Message:lj}{NewLine}",
            false);
