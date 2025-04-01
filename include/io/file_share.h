#pragma once
#include <cstdint>
#include <type_traits>
namespace ws {
namespace io {
enum class FileShare : uint8_t {
  kRead = 1 << 0,
  kWrite = 1 << 1,
  Delete = 1 << 2,
};

constexpr FileShare operator|(FileShare a, FileShare b) {
  using T = std::underlying_type_t<FileShare>;
  return static_cast<FileShare>(static_cast<T>(a) | static_cast<T>(b));
}

constexpr FileShare operator&(FileShare a, FileShare b) {
  using T = std::underlying_type_t<FileShare>;
  return static_cast<FileShare>(static_cast<T>(a) & static_cast<T>(b));
}
}  // namespace io
}  // namespace ws
