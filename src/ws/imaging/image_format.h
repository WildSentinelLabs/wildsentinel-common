#pragma once

#include <cstdint>
#include <string>

#include "ws/io/file_format.h"
namespace ws {
namespace imaging {
class ImageFormat : public ws::io::FileFormat {
 public:
  virtual ~ImageFormat() override = default;
};
}  // namespace imaging
}  // namespace ws
