#pragma once

#include <cassert>
#include <cstddef>
#include <vector>

#include "array.h"

namespace ws {
template <typename T>
struct ReadOnlySpan;

template <typename T>
struct Span {
  constexpr Span() : data(nullptr), length(0) {}

  constexpr Span(T* ptr, std::size_t len) : data(ptr), length(len) {
    assert((ptr != nullptr) ||
           (len == 0) && "If length is non-zero, pointer must not be null");
  }

  constexpr Span(T* ptr, std::size_t start, std::size_t len)
      : data(ptr + start), length(len) {
    assert(ptr != nullptr && "Pointer must not be null");
  }

  Span(const Array<T>& arr) : data(arr.data.get()), length(arr.Length()) {}

  Span(const Array<T>& arr, std::size_t start, std::size_t len)
      : data(arr.data.get() + start), length(len) {
    assert(start <= arr.Length() && "Starting index exceeds array's length");
    assert(start + len <= arr.Length() &&
           "Specified range exceeds the array boundaries");
  }

  constexpr Span(const Span& span) : data(span.data), length(span.length) {}

  constexpr Span(const Span& span, std::size_t start, std::size_t len)
      : data(span.data + start), length(len) {
    assert(start <= span.length &&
           "Starting index exceeds the original span length");
    assert(start + len <= span.length &&
           "Specified range exceeds the original span's boundaries");
  }

  constexpr static Span<T> Empty() { return Span<T>(); }

  constexpr std::size_t Length() const { return length; }

  constexpr bool IsEmpty() const { return length == 0; }

  constexpr T& operator[](std::size_t index) {
    assert(index < length && "Array index out of range");
    return data[index];
  }

  constexpr const T& operator[](std::size_t index) const {
    assert(index < length && "Array index out of range");
    return data[index];
  }

  constexpr operator T*() { return data; }

  constexpr operator const T*() const { return data; }

  constexpr T* begin() { return data; }

  constexpr T* end() { return data + length; }

  constexpr const T* begin() const { return data; }

  constexpr const T* end() const { return data + length; }

  constexpr const T* cbegin() const { return data; }

  constexpr const T* cend() const { return data + length; }

 private:
  T* data;
  std::size_t length;

  template <typename TU>
  friend struct ReadOnlySpan;
};

template <typename T>
struct ReadOnlySpan {
  constexpr ReadOnlySpan() : data(nullptr), length(0) {}

  constexpr ReadOnlySpan(const T* ptr, std::size_t len)
      : data(ptr), length(len) {
    assert((ptr != nullptr) ||
           (len == 0) && "If length is non-zero, pointer must not be null");
  }

  constexpr ReadOnlySpan(const T* ptr, std::size_t start, std::size_t len)
      : data(ptr + start), length(len) {
    assert(ptr != nullptr && "Pointer must not be null");
  }

  constexpr ReadOnlySpan(const std::vector<T>& vec)
      : data(vec.data()), length(vec.size()) {}

  constexpr ReadOnlySpan(const std::vector<T>& vec, std::size_t start,
                         std::size_t len)
      : data(vec.data() + start), length(len) {
    assert(start + len <= vec.size());
  }

  ReadOnlySpan(const Array<T>& arr) : data(arr), length(arr.Length()) {}

  ReadOnlySpan(const Array<T>& arr, std::size_t start, std::size_t len)
      : data(arr + start), length(len) {
    assert(start <= arr.Length() && "Starting index exceeds array's length");
    assert(start + len <= arr.Length() &&
           "Specified range exceeds the array boundaries");
  }

  constexpr ReadOnlySpan(const Span<T>& span)
      : data(span.data), length(span.length) {}

  constexpr ReadOnlySpan(const Span<T>& span, std::size_t start,
                         std::size_t len)
      : data(span.data + start), length(len) {
    assert(start <= span.length &&
           "Starting index exceeds the original span length");
    assert(start + len <= span.length &&
           "Specified range exceeds the original span's boundaries");
  }

  constexpr ReadOnlySpan(const ReadOnlySpan& span)
      : data(span.data), length(span.length) {}

  constexpr ReadOnlySpan(const ReadOnlySpan& span, std::size_t start,
                         std::size_t len)
      : data(span.data + start), length(len) {
    assert(start <= span.length &&
           "Starting index exceeds the original span length");
    assert(start + len <= span.length &&
           "Specified range exceeds the original span's boundaries");
  }

  constexpr static ReadOnlySpan<T> Empty() { return ReadOnlySpan<T>(); }

  constexpr std::size_t Length() const { return length; }

  constexpr bool IsEmpty() const { return length == 0; }

  constexpr const T& operator[](std::size_t index) const {
    assert(index < length && "Array index out of range");
    return data[index];
  }

  constexpr operator const T*() const { return data; }

  constexpr const T* begin() const { return data; }

  constexpr const T* end() const { return data + length; }

  constexpr const T* cbegin() const { return data; }

  constexpr const T* cend() const { return data + length; }

 private:
  const T* data;
  std::size_t length;
};

}  // namespace ws
// TODO: Add validations in contructor
