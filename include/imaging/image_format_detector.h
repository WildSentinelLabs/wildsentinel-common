#pragma once

#include "imaging/image_format.h"
#include "io/file_handling/file_format_detector.h"

class ImageFormatDetector : public FileFormatDetector {
 public:
  ~ImageFormatDetector() override = default;

  virtual const ImageFormat& Format() const override = 0;
};
