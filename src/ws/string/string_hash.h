#pragma once

#include <functional>
#include <string>
#include <string_view>

namespace ws {
struct StringEqual {
  using is_transparent = void;

  bool operator()(const std::string& lhs, const std::string& rhs) const {
    return lhs == rhs;
  }
  bool operator()(std::string_view lhs, std::string_view rhs) const {
    return lhs == rhs;
  }
  bool operator()(const std::string& lhs, std::string_view rhs) const {
    return lhs == rhs;
  }
  bool operator()(std::string_view lhs, const std::string& rhs) const {
    return lhs == rhs;
  }
};

struct StringHash {
  using is_transparent = void;
  using transparent_key_equal = StringEqual;

  std::size_t operator()(std::string_view s) const noexcept {
    return std::hash<std::string_view>{}(s);
  }

  std::size_t operator()(const std::string& s) const noexcept {
    return std::hash<std::string>{}(s);
  }
};
}  // namespace ws
