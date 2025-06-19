#pragma once

#include <cassert>
#include <cstddef>
#include <memory>

namespace ws {
template <typename T>
struct Span;

template <typename T>
struct Array {
  constexpr Array();
  Array(std::size_t length);
  Array(std::initializer_list<T> init);
  Array(T* data, std::size_t length);
  Array(Array&& other) noexcept;
  Array(const Array&) = delete;

  Array& operator=(const Array&) = delete;
  Array& operator=(Array&& other) noexcept;
  T& operator[](std::size_t index);
  const T& operator[](std::size_t index) const;
  operator T*();
  operator const T*() const;

  ~Array();

  std::size_t Length() const;
  bool Empty() const;

  T* begin();
  T* end();
  const T* begin() const;
  const T* end() const;
  const T* cbegin() const;
  const T* cend() const;

 private:
  template <typename TU>
  friend struct Span;

  T* data;
  std::size_t length;
};

// ============================================================================
// Implementation details for ReadOnlySpan<T>
// ============================================================================

template <typename T>
inline constexpr Array<T>::Array() : data(nullptr), length(0) {}

template <typename T>
inline Array<T>::Array(std::size_t length)
    : data(length > 0 ? new T[length] : nullptr), length(length) {
  assert((length == 0 || data != nullptr) &&
         "Data pointer must not be null if length > 0");
}

template <typename T>
inline Array<T>::Array(std::initializer_list<T> init)
    : data(init.size() > 0 ? new T[init.size()] : nullptr),
      length(init.size()) {
  assert((init.size() == 0 || data != nullptr) &&
         "Data pointer must not be null if initializer list is non-empty");
  std::copy(init.begin(), init.end(), data);
}

template <typename T>
inline Array<T>::Array(T* data, std::size_t length)
    : data(data), length(length) {
  assert((length == 0 || this->data != nullptr) &&
         "Raw pointer data must not be null if length > 0");
}

template <typename T>
inline Array<T>::Array(Array&& other) noexcept
    : data(other.data), length(other.length) {
  other.data = nullptr;
  other.length = 0;
}

template <typename T>
inline Array<T>::~Array() {
  delete[] data;
}

template <typename T>
inline std::size_t Array<T>::Length() const {
  return length;
}

template <typename T>
inline bool Array<T>::Empty() const {
  return length == 0;
}

template <typename T>
inline Array<T>& Array<T>::operator=(Array<T>&& other) noexcept {
  if (this != &other) {
    delete[] data;

    data = other.data;
    length = other.length;

    other.data = nullptr;
    other.length = 0;
  }

  return *this;
}

template <typename T>
inline T& Array<T>::operator[](std::size_t index) {
  assert(index < length && "Array index out of range");
  return data[index];
}

template <typename T>
inline const T& Array<T>::operator[](std::size_t index) const {
  assert(index < length && "Array index out of range");
  return data[index];
}

template <typename T>
inline Array<T>::operator T*() {
  return data;
}

template <typename T>
inline Array<T>::operator const T*() const {
  return data;
}

template <typename T>
inline T* Array<T>::begin() {
  return data;
}

template <typename T>
inline T* Array<T>::end() {
  return data + length;
}

template <typename T>
inline const T* Array<T>::begin() const {
  return data;
}

template <typename T>
inline const T* Array<T>::end() const {
  return data + length;
}

template <typename T>
inline const T* Array<T>::cbegin() const {
  return data;
}

template <typename T>
inline const T* Array<T>::cend() const {
  return data + length;
}
}  // namespace ws
