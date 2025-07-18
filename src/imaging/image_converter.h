#pragma once
#include <string>

#include "imaging/image.h"
#include "imaging/image_traits.h"
#include "logging/enrichers/thread_id_enricher.h"
#include "logging/ilogger.h"
#include "logging/logger_configuration.h"

namespace ws {
namespace imaging {

class ImageConverter {
 public:
  ImageConverter(const ImageConverter&);
  ImageConverter(ImageConverter&&) noexcept;

  ImageConverter& operator=(const ImageConverter&);
  ImageConverter& operator=(ImageConverter&&) noexcept;

  virtual ~ImageConverter() = default;

  ColorSpace GetColorSpace() const;
  ChromaSubsampling GetChromaSubsampling() const;
  virtual StatusOr<Image> Convert(const Image& source) const = 0;

  static ws::logging::LoggerConfiguration logger_configuration_;

 protected:
  ImageConverter(ColorSpace color_space, ChromaSubsampling chroma_subsampling,
                 uint8_t num_components, const std::string& source_context);

  std::unique_ptr<ws::logging::ILogger> logger_;
  std::string source_context_;
  ColorSpace color_space_;
  ChromaSubsampling chroma_subsampling_;
  uint8_t num_components_;
  uint8_t alignment_;
};

template <typename Derived>
class TypedImageConverter : public ImageConverter {
 public:
  using ImageConverter::ImageConverter;

  StatusOr<Image> Convert(const Image& source) const final;

 private:
  template <typename T>
  StatusOr<Image> DispatchType(const Image& source) const;
  StatusOr<Image> DispatchConvert(const Image& source) const;
};

// ============================================================================
// Implementation details for ImageConverter
// ============================================================================

inline ColorSpace ImageConverter::GetColorSpace() const { return color_space_; }

inline ChromaSubsampling ImageConverter::GetChromaSubsampling() const {
  return chroma_subsampling_;
}

// ============================================================================
// Implementation details for TypedImageConverter<T>
// ============================================================================
template <typename Derived>
StatusOr<Image> TypedImageConverter<Derived>::Convert(
    const Image& source) const {
  return DispatchConvert(source);
}

template <typename Derived>
template <typename T>
StatusOr<Image> TypedImageConverter<Derived>::DispatchType(
    const Image& source) const {
  return static_cast<const Derived*>(this)->template InnerConvert<T>(
      source, this->alignment_);
}

template <typename Derived>
StatusOr<Image> TypedImageConverter<Derived>::DispatchConvert(
    const Image& source) const {
  switch (source.GetComponent(0).GetBufferType()) {
    case ImageBufferType::kUInt8:
      return DispatchType<uint8_t>(source);
    case ImageBufferType::kUInt16:
      return DispatchType<uint16_t>(source);
    case ImageBufferType::kInt16:
      return DispatchType<int16_t>(source);
    case ImageBufferType::kUInt32:
      return DispatchType<uint32_t>(source);
    case ImageBufferType::kInt32:
      return DispatchType<int32_t>(source);
    default:
      return Status(StatusCode::kBadRequest,
                    "[Convert] Unsupported pixel type");
  }
}
}  // namespace imaging
}  // namespace ws
