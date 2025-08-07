#pragma once

#include <cstdint>
#include <string>

namespace ws {
namespace imaging {

enum class ImageBufferType : int8_t {
  kInt8,
  kUInt8,
  kInt16,
  kUInt16,
  kInt32,
  kUInt32,
  kUnknown
};

constexpr ImageBufferType DetermineImageBufferType(uint8_t bit_depth) {
  if (bit_depth >= 2 && bit_depth <= 8) {
    return ImageBufferType::kUInt8;
  } else if (bit_depth >= 9 && bit_depth <= 12) {
    return ImageBufferType::kInt16;
  } else if (bit_depth >= 13 && bit_depth <= 16) {
    return ImageBufferType::kUInt16;
  } else if (bit_depth >= 17 && bit_depth <= 24) {
    return ImageBufferType::kInt32;
  } else if (bit_depth >= 25 && bit_depth <= 32) {
    return ImageBufferType::kUInt32;
  } else {
    return ImageBufferType::kUnknown;
  }
}

std::string ImageBufferTypeToString(ImageBufferType type);

template <ImageBufferType>
struct ImageBufferTypeTraits;

template <>
struct ImageBufferTypeTraits<ImageBufferType::kInt8> {
  using type = int8_t;
};
template <>
struct ImageBufferTypeTraits<ImageBufferType::kUInt8> {
  using type = uint8_t;
};
template <>
struct ImageBufferTypeTraits<ImageBufferType::kInt16> {
  using type = int16_t;
};
template <>
struct ImageBufferTypeTraits<ImageBufferType::kUInt16> {
  using type = uint16_t;
};
template <>
struct ImageBufferTypeTraits<ImageBufferType::kInt32> {
  using type = int32_t;
};
template <>
struct ImageBufferTypeTraits<ImageBufferType::kUInt32> {
  using type = uint32_t;
};

template <typename T>
struct ImageBufferTypeOf;

template <>
struct ImageBufferTypeOf<int8_t> {
  static constexpr ImageBufferType value = ImageBufferType::kInt8;
};
template <>
struct ImageBufferTypeOf<uint8_t> {
  static constexpr ImageBufferType value = ImageBufferType::kUInt8;
};
template <>
struct ImageBufferTypeOf<int16_t> {
  static constexpr ImageBufferType value = ImageBufferType::kInt16;
};
template <>
struct ImageBufferTypeOf<uint16_t> {
  static constexpr ImageBufferType value = ImageBufferType::kUInt16;
};
template <>
struct ImageBufferTypeOf<int32_t> {
  static constexpr ImageBufferType value = ImageBufferType::kInt32;
};
template <>
struct ImageBufferTypeOf<uint32_t> {
  static constexpr ImageBufferType value = ImageBufferType::kUInt32;
};

}  // namespace imaging
}  // namespace ws
