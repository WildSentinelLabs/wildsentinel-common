#pragma once
#include <cstdint>
#include <type_traits>

namespace ws {
namespace imaging {
namespace pixel {

template <typename T>
concept IsAllowedPixelNumericType =
    std::is_same_v<T, std::uint8_t> || std::is_same_v<T, int16_t> ||
    std::is_same_v<T, uint16_t> || std::is_same_v<T, int32_t> ||
    std::is_same_v<T, uint32_t>;

template <typename T>
concept IsAllowedPixelComponentType =
    IsAllowedPixelNumericType<T> || std::is_same_v<T, float> ||
    std::is_same_v<T, double>;

}  // namespace pixel
}  // namespace imaging
}  // namespace ws
