#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace ws {
namespace io {

class FileFormat {
 public:
  virtual ~FileFormat() = default;
  virtual std::string Name() const = 0;
  virtual std::string DefaultMimeType() const = 0;
  virtual const std::vector<std::string> MimeTypes() const = 0;
  virtual const std::vector<std::string> FileExtensions() const = 0;
};
}  // namespace io
}  // namespace ws
