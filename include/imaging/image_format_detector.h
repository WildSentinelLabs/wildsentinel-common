#pragma once

#include "file_handling/file_format_detector.h"

class ImageFormatDetector : public FileFormatDetector {
 public:
  ~ImageFormatDetector() override = default;
};
