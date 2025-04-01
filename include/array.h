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
      : data(length > 0 ? std::make_unique<T[]>(length) : nullptr),
        length(length) {
    assert((length == 0 || data != nullptr) &&
           "Data pointer must not be null if length > 0");
  }

  Array(std::initializer_list<T> init)
      : data(init.size() > 0 ? std::make_unique<T[]>(init.size()) : nullptr),
        length(init.size()) {
    assert((init.size() == 0 || data != nullptr) &&
           "Data pointer must not be null if initializer list is non-empty");
    std::copy(init.begin(), init.end(), data.get());
  }

  Array(std::unique_ptr<T[]>&& data, std::size_t length)
      : data(std::move(data)), length(length) {
    assert((length == 0 || this->data != nullptr) &&
           "Unique_ptr data must not be null if length > 0");
  }

  Array(Array&& other) noexcept
      : data(std::move(other.data)), length(other.length) {
    other = Array::Empty();
  }

  Array(const Array&) = delete;

  static Array<T> Empty() { return Array<T>(); }

  std::size_t Length() const { return length; }

  bool IsEmpty() const { return length == 0; }

  Array& operator=(const Array&) = delete;

  Array& operator=(Array&& other) noexcept {
    if (this != &other) {
      data = std::move(other.data);
      length = other.length;
      other = Array::Empty();
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

  operator T*() { return data.get(); }

  operator const T*() const { return data.get(); }

  T* begin() { return data.get(); }

  T* end() { return data.get() + length; }

  const T* begin() const { return data.get(); }

  const T* end() const { return data.get() + length; }

  const T* cbegin() const { return data.get(); }

  const T* cend() const { return data.get() + length; }

 private:
  std::unique_ptr<T[]> data;
  std::size_t length;

  template <typename TU>
  friend struct Span;
};
}  // namespace ws
