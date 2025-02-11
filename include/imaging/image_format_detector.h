#pragma once

#include "imaging/image_format.h"
#include "io/file_handling/file_format_detector.h"

class ImageFormatDetector : public FileFormatDetector {
 public:
  ~ImageFormatDetector() override = default;

  const ImageFormat& Format() const override;
};
