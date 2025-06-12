#pragma once

#include <cassert>
#include <cstddef>
#include <memory>

namespace ws {
template <typename T>
struct Span;

template <typename T>
struct Array {
  Array() : data(nullptr), length(0) {}

  Array(std::size_t length)
      : data(length > 0 ? new T[length] : nullptr), length(length) {
    assert((length == 0 || data != nullptr) &&
           "Data pointer must not be null if length > 0");
  }

  Array(std::initializer_list<T> init)
      : data(init.size() > 0 ? new T[init.size()] : nullptr),
        length(init.size()) {
    assert((init.size() == 0 || data != nullptr) &&
           "Data pointer must not be null if initializer list is non-empty");
    std::copy(init.begin(), init.end(), data);
  }

  Array(T* data, std::size_t length) : data(data), length(length) {
    assert((length == 0 || this->data != nullptr) &&
           "Raw pointer data must not be null if length > 0");
  }

  Array(Array&& other) noexcept : data(other.data), length(other.length) {
    other.data = nullptr;
    other.length = 0;
  }

  Array(const Array&) = delete;

  ~Array() { delete[] data; }

  std::size_t Length() const { return length; }

  bool Empty() const { return length == 0; }

  Array& operator=(const Array&) = delete;

  Array& operator=(Array&& other) noexcept {
    if (this != &other) {
      delete[] data;

      data = other.data;
      length = other.length;

      other.data = nullptr;
      other.length = 0;
    }

    return *this;
  }

  T& operator[](std::size_t index) {
    assert(index < length && "Array index out of range");
    return data[index];
  }

  const T& operator[](std::size_t index) const {
    assert(index < length && "Array index out of range");
    return data[index];
  }

  operator T*() { return data; }

  operator const T*() const { return data; }

  T* begin() { return data; }

  T* end() { return data + length; }

  const T* begin() const { return data; }

  const T* end() const { return data + length; }

  const T* cbegin() const { return data; }

  const T* cend() const { return data + length; }

 private:
  T* data;
  std::size_t length;

  template <typename TU>
  friend struct Span;
};
}  // namespace ws
