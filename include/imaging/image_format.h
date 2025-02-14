#pragma once
#include <cstdint>
#include <string>

#include "io/file_format.h"
namespace ws {
namespace imaging {

class ImageFormat : public io::FileFormat {
 public:
  virtual ~ImageFormat() override = default;
};
}  // namespace imaging
}  // namespace ws
