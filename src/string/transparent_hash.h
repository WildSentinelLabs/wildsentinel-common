#pragma once

#include <functional>
#include <string_view>

namespace ws {
struct TransparentHash {
  using is_transparent = void;

  std::size_t operator()(std::string_view s) const noexcept {
    return std::hash<std::string_view>{}(s);
  }
};

struct TransparentEqual {
  using is_transparent = void;

  bool operator()(std::string_view lhs, std::string_view rhs) const noexcept {
    return lhs == rhs;
  }
};
}  // namespace ws
