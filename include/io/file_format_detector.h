#pragma once
#include <cstdint>

#include "io/file_format.h"
#include "io/stream.h"
namespace ws {
namespace io {

class FileFormatDetector {
 public:
  virtual ~FileFormatDetector() = default;

  virtual const FileFormat& Format() const = 0;

  virtual uint16_t HeaderSize() const = 0;

  virtual bool Detect(Stream& stream) const = 0;

  virtual bool Detect(const uint8_t* header, const size_t& length) const = 0;
};
}  // namespace io
}  // namespace ws
