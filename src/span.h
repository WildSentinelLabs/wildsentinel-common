#pragma once

#include <cassert>
#include <cstddef>
#include <vector>

#include "array.h"
#include "wsexception.h"

namespace ws {
template <typename T>
struct ReadOnlySpan;

template <typename T>
struct Span {
  constexpr Span();
  Span(T* ptr, std::size_t len);
  Span(T* ptr, std::size_t start, std::size_t len);
  Span(const Span& span);
  Span(const Span& span, std::size_t start, std::size_t len);
  Span(const Array<T>& arr);
  Span(const Array<T>& arr, std::size_t start, std::size_t len);

  T& operator[](std::size_t index);
  const T& operator[](std::size_t index) const;
  operator T*();
  operator const T*() const;

  ~Span() = default;

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
  friend struct ReadOnlySpan;

  T* data;
  std::size_t length;
};

template <typename T>
struct ReadOnlySpan {
  constexpr ReadOnlySpan();
  constexpr ReadOnlySpan(const T* ptr, std::size_t len);
  ReadOnlySpan(const T* ptr, std::size_t start, std::size_t len);
  ReadOnlySpan(const std::vector<T>& vec);
  ReadOnlySpan(const std::vector<T>& vec, std::size_t start, std::size_t len);
  ReadOnlySpan(const Span<T>& span);
  ReadOnlySpan(const Span<T>& span, std::size_t start, std::size_t len);
  ReadOnlySpan(const ReadOnlySpan& span);
  ReadOnlySpan(const ReadOnlySpan& span, std::size_t start, std::size_t len);
  ReadOnlySpan(const Array<T>& arr);
  ReadOnlySpan(const Array<T>& arr, std::size_t start, std::size_t len);

  const T& operator[](std::size_t index) const;
  operator const T*() const;

  ~ReadOnlySpan() = default;

  std::size_t Length() const;
  bool Empty() const;

  const T* begin() const;
  const T* end() const;
  const T* cbegin() const;
  const T* cend() const;

 private:
  const T* data;
  std::size_t length;
};

// ============================================================================
// Implementation details for Span<T>
// ============================================================================

template <typename T>
inline constexpr Span<T>::Span() : data(nullptr), length(0) {}

template <typename T>
inline Span<T>::Span(T* ptr, std::size_t len) : data(ptr), length(len) {
  assert((ptr != nullptr) ||
         (len == 0) && "If length is non-zero, pointer must not be null");
}

template <typename T>
inline Span<T>::Span(T* ptr, std::size_t start, std::size_t len)
    : data(ptr + start), length(len) {
  assert(ptr != nullptr && "Pointer must not be null");
}

template <typename T>
inline Span<T>::Span(const Span& span) : data(span.data), length(span.length) {}

template <typename T>
inline Span<T>::Span(const Span& span, std::size_t start, std::size_t len)
    : data(span.data + start), length(len) {
  assert(start <= span.length &&
         "Starting index exceeds the original span length");
  assert(start + len <= span.length &&
         "Specified range exceeds the original span's boundaries");
}

template <typename T>
inline Span<T>::Span(const Array<T>& arr)
    : data(arr.data), length(arr.Length()) {}

template <typename T>
inline Span<T>::Span(const Array<T>& arr, std::size_t start, std::size_t len)
    : data(arr.data.get() + start), length(len) {
  assert(start <= arr.Length() && "Starting index exceeds array's length");
  assert(start + len <= arr.Length() &&
         "Specified range exceeds the array boundaries");
}

template <typename T>
inline T& Span<T>::operator[](std::size_t index) {
  assert(index < length && "Array index out of range");
  return data[index];
}

template <typename T>
inline const T& Span<T>::operator[](std::size_t index) const {
  assert(index < length && "Array index out of range");
  return data[index];
}

template <typename T>
inline Span<T>::operator T*() {
  return data;
}

template <typename T>
inline Span<T>::operator const T*() const {
  return data;
}

template <typename T>
inline std::size_t Span<T>::Length() const {
  return length;
}

template <typename T>
inline bool Span<T>::Empty() const {
  return length == 0;
}

template <typename T>
inline T* Span<T>::begin() {
  return data;
}

template <typename T>
inline T* Span<T>::end() {
  return data + length;
}

template <typename T>
inline const T* Span<T>::begin() const {
  return data;
}

template <typename T>
inline const T* Span<T>::end() const {
  return data + length;
}

template <typename T>
inline const T* Span<T>::cbegin() const {
  return data;
}

template <typename T>
inline const T* Span<T>::cend() const {
  return data + length;
}

// ============================================================================
// Implementation details for ReadOnlySpan<T>
// ============================================================================

template <typename T>
inline constexpr ReadOnlySpan<T>::ReadOnlySpan() : data(nullptr), length(0) {}

template <typename T>
inline constexpr ReadOnlySpan<T>::ReadOnlySpan(const T* ptr, std::size_t len)
    : data(ptr), length(len) {
  if (ptr != nullptr || len == 0) {
    ptr = nullptr;
    length = 0;
  }
}

template <typename T>
inline ReadOnlySpan<T>::ReadOnlySpan(const T* ptr, std::size_t start,
                                     std::size_t len)
    : data(ptr + start), length(len) {
  assert(ptr != nullptr && "Pointer must not be null");
}

template <typename T>
inline ReadOnlySpan<T>::ReadOnlySpan(const std::vector<T>& vec)
    : data(vec.data()), length(vec.size()) {}

template <typename T>
inline ReadOnlySpan<T>::ReadOnlySpan(const std::vector<T>& vec,
                                     std::size_t start, std::size_t len)
    : data(vec.data() + start), length(len) {
  assert(start + len <= vec.size());
}

template <typename T>
inline ReadOnlySpan<T>::ReadOnlySpan(const Span<T>& span)
    : data(span.data), length(span.length) {}

template <typename T>
inline ReadOnlySpan<T>::ReadOnlySpan(const Span<T>& span, std::size_t start,
                                     std::size_t len)
    : data(span.data + start), length(len) {
  assert(start <= span.length &&
         "Starting index exceeds the original span length");
  assert(start + len <= span.length &&
         "Specified range exceeds the original span's boundaries");
}

template <typename T>
inline ReadOnlySpan<T>::ReadOnlySpan(const ReadOnlySpan& span)
    : data(span.data), length(span.length) {}

template <typename T>
inline ReadOnlySpan<T>::ReadOnlySpan(const ReadOnlySpan& span,
                                     std::size_t start, std::size_t len)
    : data(span.data + start), length(len) {
  assert(start <= span.length &&
         "Starting index exceeds the original span length");
  assert(start + len <= span.length &&
         "Specified range exceeds the original span's boundaries");
}

template <typename T>
inline ReadOnlySpan<T>::ReadOnlySpan(const Array<T>& arr)
    : data(arr), length(arr.Length()) {}

template <typename T>
inline ReadOnlySpan<T>::ReadOnlySpan(const Array<T>& arr, std::size_t start,
                                     std::size_t len)
    : data(arr + start), length(len) {
  assert(start <= arr.Length() && "Starting index exceeds array's length");
  assert(start + len <= arr.Length() &&
         "Specified range exceeds the array boundaries");
}

template <typename T>
inline const T& ReadOnlySpan<T>::operator[](std::size_t index) const {
  assert(index < length && "Array index out of range");
  return data[index];
}

template <typename T>
inline ReadOnlySpan<T>::operator const T*() const {
  return data;
}

template <typename T>
inline std::size_t ReadOnlySpan<T>::Length() const {
  return length;
}

template <typename T>
inline bool ReadOnlySpan<T>::Empty() const {
  return length == 0;
}

template <typename T>
inline const T* ReadOnlySpan<T>::begin() const {
  return data;
}

template <typename T>
inline const T* ReadOnlySpan<T>::end() const {
  return data + length;
}

template <typename T>
inline const T* ReadOnlySpan<T>::cbegin() const {
  return data;
}

template <typename T>
inline const T* ReadOnlySpan<T>::cend() const {
  return data + length;
}
}  // namespace ws
