#pragma once

#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

namespace ws {
// ToString overloads
inline std::string ToString(const std::string& s) { return s; }
inline std::string ToString(std::string_view s) { return std::string(s); }
inline std::string ToString(const char* s) { return std::string(s); }
inline std::string ToString(char c) { return std::string(1, c); }
inline std::string ToString(bool b) { return b ? "True" : "False"; }

// Integers
inline std::string ToString(short v) { return std::to_string(v); }
inline std::string ToString(unsigned short v) { return std::to_string(v); }
inline std::string ToString(int v) { return std::to_string(v); }
inline std::string ToString(unsigned int v) { return std::to_string(v); }
inline std::string ToString(long v) { return std::to_string(v); }
inline std::string ToString(unsigned long v) { return std::to_string(v); }
inline std::string ToString(long long v) { return std::to_string(v); }
inline std::string ToString(unsigned long long v) { return std::to_string(v); }

// Real
inline std::string ToString(float v) { return std::to_string(v); }
inline std::string ToString(double v) { return std::to_string(v); }
inline std::string ToString(long double v) { return std::to_string(v); }

// Pointers and nullptr
inline std::string ToString(std::nullptr_t) { return "nullptr"; }

inline std::string ToString(std::thread::id id) {
  std::ostringstream oss;
  oss << id;
  return oss.str();
}

template <typename T>
inline std::string ToString(T* ptr) {
  std::ostringstream oss;
  oss << static_cast<const void*>(ptr);
  return oss.str();
}

// Custom objects with ToString method
template <typename T>
auto ToString(const T& obj) -> decltype(obj.ToString(), std::string()) {
  return obj.ToString();
}

template <typename T>
inline std::string ToString(T value) {
  return std::to_string(value);
}

// Base case: no arguments
inline void FormatToImpl(std::string& out, std::string_view fmt) { out += fmt; }

// Recursive formatter
template <typename Arg, typename... Args>
void FormatToImpl(std::string& out, std::string_view fmt, Arg&& arg,
                  Args&&... args) {
  size_t pos = fmt.find("{}");
  if (pos == std::string_view::npos) {
    out += fmt;
    return;
  }

  out += fmt.substr(0, pos);
  out += ToString(std::forward<Arg>(arg));
  FormatToImpl(out, fmt.substr(pos + 2), std::forward<Args>(args)...);
}

// Entry point
template <typename... Args>
std::string Format(std::string_view fmt, Args&&... args) {
  std::string out;
  out.reserve(fmt.size() + sizeof...(args) * 8);  // heuristic
  FormatToImpl(out, fmt, std::forward<Args>(args)...);
  return out;
}
}  // namespace ws
