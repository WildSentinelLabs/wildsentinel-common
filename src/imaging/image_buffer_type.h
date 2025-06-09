#pragma once
#include <cstdint>
#include <string>
namespace ws {
namespace imaging {

enum class ImageBufferType : int8_t {
  kUInt8,
  kUInt16,
  kInt16,
  kUInt32,
  kInt32,
  kUnknown
};

ImageBufferType DetermineImageBufferType(uint8_t bit_depth);

std::string ImageBufferTypeToString(ImageBufferType type);
}  // namespace imaging
}  // namespace ws
