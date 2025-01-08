#pragma once
#include <cstdint>

#include "image_format.h"
#include "streams/stream.h"

class ImageFormatDetector {
 public:
  virtual ~ImageFormatDetector() = default;

  virtual uint16_t HeaderSize() const = 0;

  virtual bool Detect(Stream stream) const = 0;

  virtual bool Detect(const uint8_t* header) const = 0;
};
