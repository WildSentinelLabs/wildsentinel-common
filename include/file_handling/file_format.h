#pragma once
#include <cstdint>
#include <string>

class FileFormat {
 public:
  virtual ~FileFormat() = default;
  virtual std::string Name() const = 0;
  virtual std::string DefaultMimeType() const = 0;
  virtual const std::string* MimeTypesValues() const = 0;
  virtual const std::string* FileExtensionsValues() const = 0;
  virtual uint8_t MimeTypesSize() const = 0;
  virtual uint8_t FileExtensionsSize() const = 0;
};
