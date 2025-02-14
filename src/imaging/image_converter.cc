#include "imaging/image_converter.h"

#include "imaging/image_traits.h"
namespace ws {
namespace imaging {

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

ws::logging::LoggerConfiguration ImageConverter::logger_configuration_ =
    ws::logging::LoggerConfiguration()
        .SetMinimumLogLevel(ws::logging::LogLevel::kInformation)
        .AddEnricher(
            std::make_shared<ws::logging::enrichers::ThreadIdEnricher>())
        .AddConsoleSink(ws::logging::LogLevel::kVerbose,
                        "{Timestamp:%Y-%m-%d %X} [{Level:u3}] - "
                        "[ThreadId: {ThreadId}] "
                        "[{SourceContext}] {Message:lj}{NewLine}",
                        false);
}  // namespace imaging
}  // namespace ws
