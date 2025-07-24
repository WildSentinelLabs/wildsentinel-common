#pragma once

#include <cstdio>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
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

// Real numbers
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
inline auto ToString(const T& obj) -> decltype(obj.ToString(), std::string()) {
  return obj.ToString();
}

template <typename T>
inline std::string ToString(T value) {
  return std::to_string(value);
}

namespace internal {
// Format integer with padding
template <typename T>
constexpr const char* IntFmt() {
  if constexpr (std::is_same_v<T, int>)
    return "%d";
  else if constexpr (std::is_same_v<T, long>)
    return "%ld";
  else if constexpr (std::is_same_v<T, long long>)
    return "%lld";
  else if constexpr (std::is_same_v<T, unsigned>)
    return "%u";
  else if constexpr (std::is_same_v<T, unsigned long>)
    return "%lu";
  else if constexpr (std::is_same_v<T, unsigned long long>)
    return "%llu";
  else
    static_assert(std::is_integral_v<T>, "Unsupported integral type");

  return nullptr;
}

template <typename T>
inline std::string FormatPadded(T value, int width, char fill) {
  char buffer[64];
  // build e.g. "%05ld" or "%*d"
  char fmt[16];
  if (fill == '0') {
    std::snprintf(fmt, sizeof(fmt), "%%0%d%s", width, IntFmt<T>() + 1);
    std::snprintf(buffer, sizeof(buffer), fmt, value);
  } else {
    std::snprintf(fmt, sizeof(fmt), "%%*%s", IntFmt<T>() + 1);
    std::snprintf(buffer, sizeof(buffer), fmt, width, value);
  }
  return buffer;
}

// Parse format specifiers like {:03} or {:5}
inline std::optional<std::pair<int, char>> ParseFormatSpec(
    std::string_view spec) {
  if (spec.empty() || spec[0] != ':') return std::nullopt;

  int width = 0;
  char fill = ' ';
  size_t i = 1;

  if (spec[i] == '0') {
    fill = '0';
    ++i;
  }

  while (i < spec.size() && std::isdigit(spec[i])) {
    width = width * 10 + (spec[i] - '0');
    ++i;
  }

  if (width > 0) return std::make_pair(width, fill);
  return std::nullopt;
}

// Base case: no arguments
inline void FormatToImpl(std::string& out, std::string_view fmt) {
  size_t pos = 0;
  while (pos < fmt.size()) {
    size_t found = fmt.find("{{", pos);
    if (found == std::string_view::npos) {
      found = fmt.find("}}", pos);
      if (found == std::string_view::npos) {
        out += fmt.substr(pos);
        break;
      }
      out += fmt.substr(pos, found - pos);
      out += '}';
      pos = found + 2;
    } else {
      out += fmt.substr(pos, found - pos);
      out += '{';
      pos = found + 2;
    }
  }
}

// Recursive formatter
template <typename Arg, typename... Args>
inline void FormatToImpl(std::string& out, std::string_view fmt, Arg&& arg,
                         Args&&... args) {
  size_t pos = 0;

  while (pos < fmt.size()) {
    size_t open = fmt.find('{', pos);
    if (open == std::string_view::npos) {
      FormatToImpl(out, fmt.substr(pos));
      return;
    }

    if (open + 1 < fmt.size() && fmt[open + 1] == '{') {
      out += fmt.substr(pos, open - pos);
      out += '{';
      pos = open + 2;
      continue;
    }

    size_t close = fmt.find('}', open);
    if (close == std::string_view::npos) {
      FormatToImpl(out, fmt.substr(pos));
      return;
    }

    out += fmt.substr(pos, open - pos);
    std::string_view spec = fmt.substr(open + 1, close - open - 1);
    auto fmt_spec = ParseFormatSpec(spec);
    if constexpr (std::is_integral_v<std::decay_t<Arg>>) {
      if (fmt_spec && !spec.empty()) {
        out += FormatPadded(arg, fmt_spec->first, fmt_spec->second);
      } else {
        out += ToString(std::forward<Arg>(arg));
      }
    } else {
      out += ToString(std::forward<Arg>(arg));
    }

    FormatToImpl(out, fmt.substr(close + 1), std::forward<Args>(args)...);
    return;
  }
}

}  // namespace internal

// Entry point
template <typename... Args>
inline std::string Format(std::string_view fmt, Args&&... args) {
  std::string out;
  out.reserve(fmt.size() + sizeof...(args) * 8);  // heuristic
  ws::internal::FormatToImpl(out, fmt, std::forward<Args>(args)...);
  return out;
}
}  // namespace ws
