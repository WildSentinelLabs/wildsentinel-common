#pragma once

#include <cstdint>
#include <string>
namespace ws {
namespace imaging {

enum class ChromaSubsampling : int8_t {
  kSamp444 = 0,  // None
  kSamp422 = 1,  // Horizontal
  kSamp420 = 2,  // Both
  kSamp400 = 3,  // GRAYSCALE
                 // OTHERS
  kSamp440 = 4,
  kSamp411 = 5,
  kSamp441 = 6,
  kUnsupported = -1,
};

const std::string ChromaSubsamplingToString(
    const ChromaSubsampling& subsampling);
}  // namespace imaging
}  // namespace ws
