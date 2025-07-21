#pragma once

#include <cstdint>
#include <string>
namespace ws {
namespace imaging {
enum class ImageCompressionType : int8_t {
  kLossy = 0,
  kLossless = 1,
  kUnsupported = -1,
};

const std::string ImageCompressionTypeToString(
    const ImageCompressionType& encoding_type);
}  // namespace imaging
}  // namespace ws
