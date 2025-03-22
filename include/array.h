#pragma once

#include <cstddef>
#include <memory>

namespace ws {
template <typename T>
struct Array {
  const std::size_t length;

  Array(std::size_t length)
      : data(std::make_unique<T[]>(length)), length(length) {}

  T& operator[](std::size_t index) { return data[index]; }

  const T& operator[](std::size_t index) const { return data[index]; }

 private:
  std::unique_ptr<T[]> data;
};
}  // namespace ws
