#pragma once
#include <string>

#include "ws/imaging/image.h"
#include "ws/imaging/image_traits.h"
#include "ws/logging/ilogger.h"

namespace ws {
namespace imaging {

class ImageConverter {
 public:
  ImageConverter(const ImageConverter&) = delete;
  ImageConverter(ImageConverter&&) noexcept;

  ImageConverter& operator=(const ImageConverter&) = delete;
  ImageConverter& operator=(ImageConverter&&) noexcept;

  virtual ~ImageConverter() = default;

  constexpr uint8_t NumComponents() const;
  constexpr ColorSpace GetColorSpace() const;
  constexpr ChromaSubsampling GetChromaSubsampling() const;
  void SetLogger(
      std::unique_ptr<ws::logging::ILoggerOf<ImageConverter>>&& logger);
  virtual StatusOr<Image> Convert(const Image& source) const = 0;

 protected:
  ImageConverter(ColorSpace color_space, ChromaSubsampling chroma_subsampling,
                 uint8_t num_components,
                 std::unique_ptr<ws::logging::ILoggerOf<ImageConverter>>&&
                     logger = nullptr);

  std::unique_ptr<ws::logging::ILoggerOf<ImageConverter>> logger_;
  ColorSpace color_space_;
  ChromaSubsampling chroma_subsampling_;
  uint8_t num_components_;
  uint8_t alignment_;
};

template <typename Derived>
class TypedImageConverter : public ImageConverter {
 public:
  template <typename... Args>
  TypedImageConverter(Args&&... args);

  StatusOr<Image> Convert(const Image& source) const final;

 private:
  template <typename T>
  StatusOr<Image> DispatchType(const Image& source) const;
  StatusOr<Image> DispatchConvert(const Image& source) const;
};

// ============================================================================
// Implementation details for ImageConverter
// ============================================================================
inline constexpr uint8_t ImageConverter::NumComponents() const {
  return num_components_;
}

inline constexpr ColorSpace ImageConverter::GetColorSpace() const {
  return color_space_;
}

inline constexpr ChromaSubsampling ImageConverter::GetChromaSubsampling()
    const {
  return chroma_subsampling_;
}

// ============================================================================
// Implementation details for TypedImageConverter<T>
// ============================================================================

template <typename Derived>
template <typename... Args>
inline TypedImageConverter<Derived>::TypedImageConverter(Args&&... args)
    : ImageConverter(std::forward<Args>(args)...) {}

template <typename Derived>
inline StatusOr<Image> TypedImageConverter<Derived>::Convert(
    const Image& source) const {
  return DispatchConvert(source);
}

template <typename Derived>
template <typename T>
inline StatusOr<Image> TypedImageConverter<Derived>::DispatchType(
    const Image& source) const {
  return static_cast<const Derived*>(this)->template InnerConvert<T>(
      source, this->alignment_);
}

template <typename Derived>
inline StatusOr<Image> TypedImageConverter<Derived>::DispatchConvert(
    const Image& source) const {
  if (logger_)
    logger_->LogDebug(
        "Initializing ImageConverter [ChromaSubsampling: {}, ColorSpace: {}, "
        "Components: {}]",
        ChromaSubsamplingToString(chroma_subsampling_),
        ColorSpaceToString(color_space_), num_components_);
  Image image;
  switch (source.GetComponent(0).GetBufferType()) {
    case ImageBufferType::kUInt8:
      ASSIGN_OR_RETURN(image, DispatchType<uint8_t>(source));
      break;
    case ImageBufferType::kUInt16:
      ASSIGN_OR_RETURN(image, DispatchType<uint16_t>(source));
      break;
    case ImageBufferType::kInt16:
      ASSIGN_OR_RETURN(image, DispatchType<int16_t>(source));
      break;
    case ImageBufferType::kUInt32:
      ASSIGN_OR_RETURN(image, DispatchType<uint32_t>(source));
      break;
    case ImageBufferType::kInt32:
      ASSIGN_OR_RETURN(image, DispatchType<int32_t>(source));
      break;
    default:
      return Status(StatusCode::kBadRequest,
                    "[Convert] Unsupported pixel type");
  }

  if (logger_) logger_->LogDebug(image.ToString());
  if (logger_) logger_->LogDebug("Done.");
  return image;
}
}  // namespace imaging
}  // namespace ws
