#pragma once
#include <cstdint>
#include <span>
#include <string>

namespace ws {
namespace io {

class FileFormat {
 public:
  virtual ~FileFormat() = default;

  virtual std::string Name() const = 0;
  virtual std::string DefaultMimeType() const = 0;
  virtual std::span<const std::string> MimeTypes() const = 0;
  virtual std::span<const std::string> FileExtensions() const = 0;
};
}  // namespace io
}  // namespace ws
