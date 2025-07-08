#include "imaging/image_converter.h"
namespace ws {
namespace imaging {

ImageConverter::ImageConverter(ColorSpace color_space,
                               ChromaSubsampling chroma_subsampling,
                               uint8_t num_components,
                               const std::string& source_context)
    : color_space_(color_space),
      chroma_subsampling_(chroma_subsampling),
      num_components_(num_components),
      alignment_(ImageTraits::GetChromaAlignment(chroma_subsampling)),
      source_context_(source_context),
      logger_(logger_configuration_.CreateLogger(source_context)) {};

ImageConverter::ImageConverter(const ImageConverter& other)
    : color_space_(other.color_space_),
      chroma_subsampling_(other.chroma_subsampling_),
      num_components_(other.num_components_),
      alignment_(other.alignment_),
      source_context_(other.source_context_),
      logger_(logger_configuration_.CreateLogger(source_context_)) {}

ImageConverter::ImageConverter(ImageConverter&& other) noexcept
    : color_space_(other.color_space_),
      chroma_subsampling_(other.chroma_subsampling_),
      num_components_(other.num_components_),
      alignment_(other.alignment_),
      source_context_(std::move(other.source_context_)),
      logger_(std::move(other.logger_)) {
  other.color_space_ = ColorSpace::kUnsupported;
  other.chroma_subsampling_ = ChromaSubsampling::kUnsupported;
  other.num_components_ = 0;
  other.alignment_ = 0;
}

ImageConverter& ImageConverter::operator=(const ImageConverter& other) {
  if (this != &other) {
    color_space_ = other.color_space_;
    chroma_subsampling_ = other.chroma_subsampling_;
    num_components_ = other.num_components_;
    alignment_ = other.alignment_;
    source_context_ = other.source_context_;
    logger_ = logger_configuration_.CreateLogger(source_context_);
  }

  return *this;
}

ImageConverter& ImageConverter::operator=(ImageConverter&& other) noexcept {
  if (this != &other) {
    color_space_ = other.color_space_;
    chroma_subsampling_ = other.chroma_subsampling_;
    num_components_ = other.num_components_;
    alignment_ = other.alignment_;
    source_context_ = std::move(other.source_context_);
    logger_ = std::move(other.logger_);

    other.color_space_ = ColorSpace::kUnsupported;
    other.chroma_subsampling_ = ChromaSubsampling::kUnsupported;
    other.num_components_ = 0;
    other.alignment_ = 0;
  }

  return *this;
}

ws::logging::LoggerConfiguration ImageConverter::logger_configuration_ =
    ws::logging::LoggerConfiguration()
        .SetMinimumLogLevel(ws::logging::LogLevel::kInformation)
        .AddEnricher<ws::logging::ThreadIdEnricher>()
        .AddConsoleSink(ws::logging::LogLevel::kVerbose,
                        "{Timestamp:%Y-%m-%d %X} [{Level:u3}] - "
                        "[ThreadId: {ThreadId}] "
                        "[{SourceContext}] {Message:lj}{NewLine}",
                        false);
}  // namespace imaging
}  // namespace ws
