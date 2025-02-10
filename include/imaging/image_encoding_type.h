#pragma once
#include <cstdint>
#include <string>

enum class ImageEncodingType : int8_t {
  kLossy = 0,
  kLossless = 1,
  kUnsupported = -1,
};

const std::string EncodingTypeToString(const ImageEncodingType& encoding_type);

std::ostream& operator<<(std::ostream& os,
                         const ImageEncodingType& encoding_type);
