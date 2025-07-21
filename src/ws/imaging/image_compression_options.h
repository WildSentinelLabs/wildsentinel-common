#pragma once

#include <optional>

#include "ws/imaging/image_compression_type.h"
#include "ws/string/format.h"

namespace ws {
namespace imaging {
struct ImageCompressionOptions {
  ImageCompressionType type = ImageCompressionType::kLossy;
  std::optional<int> quality = 100;

  std::string ToString() const;
};
}  // namespace imaging
}  // namespace ws
