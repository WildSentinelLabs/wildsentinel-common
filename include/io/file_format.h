#pragma once
#include <cstdint>
#include <string>

#include "span.h"

namespace ws {
namespace io {

class FileFormat {
 public:
  virtual ~FileFormat() = default;
  virtual std::string Name() const = 0;
  virtual std::string DefaultMimeType() const = 0;
  virtual const ReadOnlySpan<std::string> MimeTypes() const = 0;
  virtual const ReadOnlySpan<std::string> FileExtensions() const = 0;
};
}  // namespace io
}  // namespace ws
