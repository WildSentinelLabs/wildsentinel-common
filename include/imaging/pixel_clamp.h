#pragma once

#include <type_traits>

class PixelClamp {
 public:
  template <typename T, typename = typename std::enable_if<
                            std::is_arithmetic<T>::value>::type>
  static constexpr T Clamp(T value, T min_value, T max_value) {
    return value < min_value ? min_value
                             : (value > max_value ? max_value : value);
  }
};
