#pragma once
#include <cstdint>
#include <string>
namespace ws {
namespace imaging {

enum class ImageEncodingType : int8_t {
  kLossy = 0,
  kLossless = 1,
  kUnsupported = -1,
};

const std::string ImageEncodingTypeToString(
    const ImageEncodingType& encoding_type);

std::ostream& operator<<(std::ostream& os,
                         const ImageEncodingType& encoding_type);
}  // namespace imaging
}  // namespace ws
