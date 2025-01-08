#pragma once
#include <string>

class ImageFormat {
 public:
  virtual std::string Name() const = 0;
  virtual std::string DefaultMimeType() const = 0;
  virtual std::string* MimeTypesValues() const = 0;
  virtual std::string* FileExtensionsValues() const = 0;
  virtual uint8_t MimeTypesSize() const = 0;
  virtual uint8_t FileExtensionsSize() const = 0;
};
