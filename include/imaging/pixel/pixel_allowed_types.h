#pragma once
#include <concepts>
#include <cstdint>
#include <type_traits>

namespace ws {
namespace imaging {
namespace pixel {

template <typename T>
concept IsAllowedPixelNumericType =
    std::integral<T> && (sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4);

template <typename T>
concept IsAllowedPixelComponentType =
    IsAllowedPixelNumericType<T> || std::floating_point<T> ||
    std::is_same_v<T, double>;

}  // namespace pixel
}  // namespace imaging
}  // namespace ws
