#pragma once
#include <cstdint>
#include <span>

#include "ws/io/file_format.h"
#include "ws/io/stream.h"
namespace ws {
namespace io {

class FileFormatDetector {
 public:
  virtual ~FileFormatDetector() = default;

  virtual const FileFormat& Format() const = 0;
  virtual uint16_t HeaderSize() const = 0;
  virtual StatusOr<bool> Detect(Stream& stream) const = 0;
  virtual bool Detect(std::span<const unsigned char> header) const = 0;
};
}  // namespace io
}  // namespace ws
