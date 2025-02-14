#pragma once

#include "imaging/image_format.h"
#include "io/file_format_detector.h"
namespace ws {
namespace imaging {

class ImageFormatDetector : public ws::io::FileFormatDetector {
 public:
  ~ImageFormatDetector() override = default;

  virtual const ImageFormat& Format() const override = 0;
};
}  // namespace imaging
}  // namespace ws
