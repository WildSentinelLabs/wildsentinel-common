#pragma once
#include <cstdint>
#include <string>

#include "io/file_handling/file_format.h"

class ImageFormat : public FileFormat {
 public:
  virtual ~ImageFormat() override = default;
};
