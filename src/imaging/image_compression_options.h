#pragma once

#include <optional>

#include "imaging/image_compression_type.h"
#include "string/format.h"

namespace ws {
namespace imaging {
struct ImageCompressionOptions {
  ImageCompressionType type = ImageCompressionType::kLossy;
  std::optional<int> quality = 100;

  std::string ToString() const;
};
}  // namespace imaging
}  // namespace ws
