#pragma once
#include <concepts>
#include <cstdint>
#include <type_traits>

namespace ws {
namespace imaging {
namespace pixel {

template <typename T>
concept IsAllowedPixelNumericType =
    std::is_same_v<T, std::uint8_t> || std::is_same_v<T, std::int8_t> ||
    std::is_same_v<T, uint16_t> || std::is_same_v<T, int16_t> ||
    std::is_same_v<T, uint32_t> || std::is_same_v<T, int32_t>;
//     std::integral<T> && (sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) ==
//     4);
// TODO: Disable warning with the commented concepts

}  // namespace pixel
}  // namespace imaging
}  // namespace ws
