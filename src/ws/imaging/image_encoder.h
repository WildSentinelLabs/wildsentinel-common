#pragma once

#include "ws/imaging/image.h"
#include "ws/imaging/image_compression_options.h"
#include "ws/imaging/image_format.h"
#include "ws/io/stream.h"
#include "ws/logging/ilogger.h"
#include "ws/status/status.h"

namespace ws {
namespace imaging {
class ImageEncoder {
 public:
  ImageEncoder(const ImageEncoder&) = delete;
  ImageEncoder(ImageEncoder&&) noexcept;

  ImageEncoder& operator=(const ImageEncoder&) = delete;
  ImageEncoder& operator=(ImageEncoder&&) noexcept;

  virtual ~ImageEncoder() = default;

  ImageCompressionOptions Options() const;
  void SetLogger(
      std::unique_ptr<ws::logging::ILoggerOf<ImageEncoder>>&& logger);
  virtual void SetOptions(const ImageCompressionOptions& options);
  virtual const ImageFormat& Format() const = 0;
  virtual Status Encode(const Image& image, ws::io::Stream& stream) const = 0;

 protected:
  ImageEncoder(
      const ImageContext& context,
      const ImageCompressionOptions& compression_options = {},
      std::unique_ptr<ws::logging::ILoggerOf<ImageEncoder>>&& logger = nullptr);

  std::unique_ptr<ws::logging::ILoggerOf<ImageEncoder>> logger_;
  ImageCompressionOptions compression_options_;
  ImageContext context_;
};

// ============================================================================
// Implementation details for ImageEncoder<T>
// ============================================================================

inline ImageCompressionOptions ImageEncoder::Options() const {
  return compression_options_;
}
}  // namespace imaging
}  // namespace ws
