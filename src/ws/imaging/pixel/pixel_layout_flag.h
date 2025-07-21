#pragma once

#include <cstdint>
#include <type_traits>

namespace ws {
namespace imaging {

enum class PixelLayoutFlag : uint8_t {
  kInterleaved = 1 << 0,  // 0x01
  kPlanar = 1 << 1        // 0x02
};

constexpr PixelLayoutFlag operator|(PixelLayoutFlag a, PixelLayoutFlag b) {
  using T = std::underlying_type_t<PixelLayoutFlag>;
  return static_cast<PixelLayoutFlag>(static_cast<T>(a) | static_cast<T>(b));
}

constexpr PixelLayoutFlag operator&(PixelLayoutFlag a, PixelLayoutFlag b) {
  using T = std::underlying_type_t<PixelLayoutFlag>;
  return static_cast<PixelLayoutFlag>(static_cast<T>(a) & static_cast<T>(b));
}

}  // namespace imaging
}  // namespace ws
