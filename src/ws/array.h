#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <span>
#include <stdexcept>

namespace ws {
template <typename T>
struct Array {
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = T&;
  using const_reference = const T&;
  using pointer = T*;
  using const_pointer = const T*;
  using iterator = T*;
  using const_iterator = const T*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  constexpr Array();
  explicit Array(size_type length);
  Array(std::initializer_list<value_type> init);
  Array(pointer buff, size_type length);
  Array(Array&&) noexcept;
  Array(const Array&) = delete;

  Array& operator=(const Array&) = delete;
  Array& operator=(Array&&) noexcept;

  reference operator[](size_type index);
  const_reference operator[](size_type index) const;
  reference at(size_type index);
  const_reference at(size_type index) const;
  reference front();
  const_reference front() const;
  reference back();
  const_reference back() const;

  ~Array();

  [[nodiscard]] bool empty() const noexcept;
  size_type size() const noexcept;
  size_type max_size() const noexcept;

  pointer data() noexcept;
  const_pointer data() const noexcept;

  iterator begin() noexcept;
  iterator end() noexcept;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;
  reverse_iterator rbegin() noexcept;
  reverse_iterator rend() noexcept;
  const_reverse_iterator rbegin() const noexcept;
  const_reverse_iterator rend() const noexcept;
  const_reverse_iterator crbegin() const noexcept;
  const_reverse_iterator crend() const noexcept;

 private:
  pointer buffer;
  size_type length;
};

// ============================================================================
// Implementation details for Array<T>
// ============================================================================

template <typename T>
inline constexpr Array<T>::Array() : buffer(nullptr), length(0) {}

template <typename T>
inline Array<T>::Array(size_type length)
    : buffer(length > 0 ? new value_type[length] : nullptr), length(length) {
  assert((length == 0 || buffer != nullptr) &&
         "Buffer pointer must not be null if length > 0");
}

template <typename T>
inline Array<T>::Array(std::initializer_list<value_type> init)
    : buffer(init.size() > 0 ? new value_type[init.size()] : nullptr),
      length(init.size()) {
  assert((init.size() == 0 || buffer != nullptr) &&
         "Buffer pointer must not be null if initializer list is non-empty");
  std::copy(init.begin(), init.end(), buffer);
}

template <typename T>
inline Array<T>::Array(pointer buff, size_type length)
    : buffer(buff), length(length) {
  assert((length == 0 || this->buffer != nullptr) &&
         "Raw pointer buffer must not be null if length > 0");
}

template <typename T>
inline Array<T>::Array(Array&& other) noexcept
    : buffer(other.buffer), length(other.length) {
  other.buffer = nullptr;
  other.length = 0;
}

template <typename T>
inline Array<T>::~Array() {
  delete[] buffer;
}

template <typename T>
inline bool Array<T>::empty() const noexcept {
  return length == 0;
}

template <typename T>
inline typename Array<T>::size_type Array<T>::size() const noexcept {
  return length;
}

template <typename T>
inline typename Array<T>::size_type Array<T>::max_size() const noexcept {
  return std::numeric_limits<size_type>::max() / sizeof(value_type);
}

template <typename T>
inline typename Array<T>::pointer Array<T>::data() noexcept {
  return buffer;
}

template <typename T>
inline typename Array<T>::const_pointer Array<T>::data() const noexcept {
  return buffer;
}

template <typename T>
inline Array<T>& Array<T>::operator=(Array<T>&& other) noexcept {
  if (this != &other) {
    delete[] buffer;

    buffer = other.buffer;
    length = other.length;

    other.buffer = nullptr;
    other.length = 0;
  }

  return *this;
}

template <typename T>
inline typename Array<T>::reference Array<T>::operator[](size_type index) {
  assert(index < length && "Array index out of range");
  return buffer[index];
}

template <typename T>
inline typename Array<T>::const_reference Array<T>::operator[](
    size_type index) const {
  assert(index < length && "Array index out of range");
  return buffer[index];
}

template <typename T>
inline typename Array<T>::reference Array<T>::at(size_type index) {
  if (index >= length) throw std::out_of_range("Array::at: index out of range");
  return buffer[index];
}

template <typename T>
inline typename Array<T>::const_reference Array<T>::at(size_type index) const {
  if (index >= length) throw std::out_of_range("Array::at: index out of range");
  return buffer[index];
}

template <typename T>
inline typename Array<T>::reference Array<T>::front() {
  assert(length > 0 && "Array::front called on empty array");
  return buffer[0];
}

template <typename T>
inline typename Array<T>::const_reference Array<T>::front() const {
  assert(length > 0 && "Array::front called on empty array");
  return buffer[0];
}

template <typename T>
inline typename Array<T>::reference Array<T>::back() {
  assert(length > 0 && "Array::back called on empty array");
  return buffer[length - 1];
}

template <typename T>
inline typename Array<T>::const_reference Array<T>::back() const {
  assert(length > 0 && "Array::back called on empty array");
  return buffer[length - 1];
}

template <typename T>
inline typename Array<T>::iterator Array<T>::begin() noexcept {
  return buffer;
}

template <typename T>
inline typename Array<T>::iterator Array<T>::end() noexcept {
  return buffer + length;
}

template <typename T>
inline typename Array<T>::const_iterator Array<T>::begin() const noexcept {
  return buffer;
}

template <typename T>
inline typename Array<T>::const_iterator Array<T>::end() const noexcept {
  return buffer + length;
}

template <typename T>
inline typename Array<T>::const_iterator Array<T>::cbegin() const noexcept {
  return buffer;
}

template <typename T>
inline typename Array<T>::const_iterator Array<T>::cend() const noexcept {
  return buffer + length;
}

template <typename T>
inline typename Array<T>::reverse_iterator Array<T>::rbegin() noexcept {
  return reverse_iterator(end());
}

template <typename T>
inline typename Array<T>::reverse_iterator Array<T>::rend() noexcept {
  return reverse_iterator(begin());
}

template <typename T>
inline typename Array<T>::const_reverse_iterator Array<T>::rbegin()
    const noexcept {
  return const_reverse_iterator(end());
}

template <typename T>
inline typename Array<T>::const_reverse_iterator Array<T>::rend()
    const noexcept {
  return const_reverse_iterator(begin());
}

template <typename T>
inline typename Array<T>::const_reverse_iterator Array<T>::crbegin()
    const noexcept {
  return const_reverse_iterator(cend());
}

template <typename T>
inline typename Array<T>::const_reverse_iterator Array<T>::crend()
    const noexcept {
  return const_reverse_iterator(cbegin());
}

template <typename T>
inline bool operator==(const Array<T>& lhs, const Array<T>& rhs) {
  return lhs.size() == rhs.size() &&
         std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T>
inline bool operator!=(const Array<T>& lhs, const Array<T>& rhs) {
  return !(lhs == rhs);
}

template <typename T>
inline bool operator<(const Array<T>& lhs, const Array<T>& rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                      rhs.end());
}

template <typename T>
inline bool operator<=(const Array<T>& lhs, const Array<T>& rhs) {
  return !(rhs < lhs);
}

template <typename T>
inline bool operator>(const Array<T>& lhs, const Array<T>& rhs) {
  return rhs < lhs;
}

template <typename T>
inline bool operator>=(const Array<T>& lhs, const Array<T>& rhs) {
  return !(lhs < rhs);
}
}  // namespace ws
