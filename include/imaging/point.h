#pragma once

#include "types.h"

namespace ws {
namespace imaging {
struct Point {
  offset_t x;
  offset_t y;

  constexpr Point() : x(0), y(0) {}

  constexpr Point(offset_t x, offset_t y) : x(x), y(y) {}
};
}  // namespace imaging
}  // namespace ws
