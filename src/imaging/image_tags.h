#pragma once

#include <string_view>
namespace ws {
namespace imaging {
struct ImageTags {
  static constexpr std::string_view kXDensity = "X_DENSITY";
  static constexpr std::string_view kYDensity = "Y_DENSITY";
  static constexpr std::string_view kDensityUnits = "DENSITY_UNITS";
};
}  // namespace imaging
}  // namespace ws
