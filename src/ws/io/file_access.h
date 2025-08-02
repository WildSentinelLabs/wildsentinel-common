#pragma once
#include <cstdint>
#include <type_traits>
namespace ws {
namespace io {
enum class FileAccess : int8_t {
  kRead = 1 << 0,
  kWrite = 1 << 1,
};

constexpr FileAccess operator|(FileAccess a, FileAccess b) {
  using T = std::underlying_type_t<FileAccess>;
  return static_cast<FileAccess>(static_cast<T>(a) | static_cast<T>(b));
}

constexpr FileAccess operator&(FileAccess a, FileAccess b) {
  using T = std::underlying_type_t<FileAccess>;
  return static_cast<FileAccess>(static_cast<T>(a) & static_cast<T>(b));
}
}  // namespace io
}  // namespace ws
