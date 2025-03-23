#pragma once

#include <cstddef>
#include <memory>

namespace ws {
template <typename T>
struct Array {
  Array(std::size_t length)
      : data(std::make_unique<T[]>(length)), length(length) {}

  Array(const Array&) = delete;

  Array(Array&& other) noexcept
      : length(other.length), data(std::move(other.data)) {}

  std::size_t Length() const { return length; }

  Array& operator=(const Array&) = delete;

  Array& operator=(Array&& other) noexcept = delete;

  T& operator[](std::size_t index) { return data[index]; }

  const T& operator[](std::size_t index) const { return data[index]; }

  operator T*() { return data.get(); }

  operator const T*() const { return data.get(); }

 private:
  std::unique_ptr<T[]> data;
  std::size_t length;
};
}  // namespace ws
