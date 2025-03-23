#pragma once

#include <cstddef>

#include "array.h"

namespace ws {

template <typename T>
struct Span {
  Span(T* ptr, std::size_t len) : data(ptr), length(len) {}

  Span(T* ptr, std::size_t start, std::size_t len)
      : data(ptr + start), length(len) {}

  Span(Array<T>& arr) : data(arr), length(arr.Length()) {}

  Span(Array<T>& arr, std::size_t len) : data(arr), length(len) {}

  Span(Array<T>& arr, std::size_t start, std::size_t len)
      : data(arr + start), length(len) {}

  Span(Span& span) : data(span.data), length(span.length) {}

  Span(Span& span, std::size_t len) : data(span.data), length(len) {}

  Span(Span& span, std::size_t start, std::size_t len)
      : data(span.data + start), length(len) {}

  std::size_t Length() const { return length; }

  T& operator[](std::size_t index) { return data[index]; }

  const T& operator[](std::size_t index) const { return data[index]; }

  operator T*() { return data; }

  operator const T*() const { return data; }

 private:
  T* data;
  std::size_t length;
};

}  // namespace ws
// TODO: Add validations in contructor
