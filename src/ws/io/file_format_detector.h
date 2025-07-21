#pragma once
#include <cstdint>

#include "ws/io/file_format.h"
#include "ws/io/stream.h"
#include "ws/span.h"
namespace ws {
namespace io {

class FileFormatDetector {
 public:
  virtual ~FileFormatDetector() = default;

  virtual const FileFormat& Format() const = 0;
  virtual uint16_t HeaderSize() const = 0;
  virtual bool Detect(Stream& stream) const = 0;
  virtual bool Detect(ReadOnlySpan<unsigned char> header) const = 0;
};
}  // namespace io
}  // namespace ws
