#pragma once
#include <cstdint>

#include "io/streams/stream.h"

class FileFormatDetector {
 public:
  virtual ~FileFormatDetector() = default;

  virtual uint16_t HeaderSize() const = 0;

  virtual bool Detect(Stream& stream) const = 0;

  virtual bool Detect(const uint8_t* header, const size_t& length) const = 0;
};
