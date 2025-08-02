#pragma once

#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>
#include <vector>

#include "ws/array.h"

namespace ws {

inline std::string ToUpper(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return std::toupper(c); });
  return s;
}

inline std::string ToLower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return s;
}

inline std::string TrimLeft(std::string s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
          }));
  return s;
}

inline std::string TrimRight(std::string s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](unsigned char ch) { return !std::isspace(ch); })
              .base(),
          s.end());
  return s;
}

inline std::string Trim(std::string s) {
  return TrimLeft(TrimRight(std::move(s)));
}

inline bool StartsWith(std::string_view str, std::string_view prefix) {
  return str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix;
}

inline bool EndsWith(std::string_view str, std::string_view suffix) {
  return str.size() >= suffix.size() &&
         str.substr(str.size() - suffix.size()) == suffix;
}

inline bool Contains(std::string_view str, std::string_view substr) {
  return str.find(substr) != std::string_view::npos;
}

inline std::string NormalizeLineEndings(std::string str) {
  if (str.find('\r') == std::string::npos) return str;
  std::string result;
  result.reserve(str.length());
  for (std::size_t i = 0; i < str.length(); ++i) {
    if (str[i] == '\r') {
      if (i + 1 < str.length() && str[i + 1] == '\n') {
        result.push_back('\n');
        ++i;
      } else {
        result.push_back(str[i]);
      }
    } else {
      result.push_back(str[i]);
    }
  }

  return result;
}

inline void NormalizeLineEndingsInPlace(std::string& str) {
  if (str.find('\r') == std::string::npos) return;
  std::size_t write_pos = 0;
  for (std::size_t read_pos = 0; read_pos < str.length(); ++read_pos) {
    if (str[read_pos] == '\r') {
      if (read_pos + 1 < str.length() && str[read_pos + 1] == '\n') {
        str[write_pos++] = '\n';
        ++read_pos;
      } else {
        str[write_pos++] = str[read_pos];
      }
    } else {
      str[write_pos++] = str[read_pos];
    }
  }

  str.resize(write_pos);
}

inline std::string DetectLineEnding(std::string_view str) {
  for (std::size_t i = 0; i < str.length(); ++i) {
    if (str[i] == '\r') {
      if (i + 1 < str.length() && str[i + 1] == '\n') {
        return "\r\n";
      }
    } else if (str[i] == '\n') {
      return "\n";
    }
  }
  return "\n";
}

inline Array<std::string> Split(std::string_view str, char delimiter) {
  std::size_t estimated_count = 1;
  for (char c : str) {
    if (c == delimiter) ++estimated_count;
  }

  Array<std::string> tokens(estimated_count);
  std::size_t index = 0;
  std::string::size_type start = 0;
  std::string::size_type end = str.find(delimiter);
  while (end != std::string::npos) {
    tokens[index++] = std::string(str.substr(start, end - start));
    start = end + 1;
    end = str.find(delimiter, start);
  }

  tokens[index] = std::string(str.substr(start));
  return tokens;
}

inline std::vector<std::string> Split(std::string_view str,
                                      std::string_view delimiters) {
  std::vector<std::string> tokens;
  std::size_t estimated_count = 1;
  for (char c : str) {
    if (delimiters.find(c) != std::string_view::npos) ++estimated_count;
  }

  tokens.reserve(estimated_count / 2 + 1);
  std::string::size_type start = 0;
  std::string::size_type end = str.find_first_of(delimiters);
  while (end != std::string::npos) {
    if (end != start) {
      tokens.emplace_back(str.substr(start, end - start));
    }
    start = end + 1;
    end = str.find_first_of(delimiters, start);
  }

  if (start < str.length()) {
    tokens.emplace_back(str.substr(start));
  }

  return tokens;
}

template <typename Container>
inline std::string Join(const Container& container,
                        std::string_view delimiter) {
  if (container.empty()) return {};

  std::string result;
  auto it = container.begin();
  result += *it;

  for (++it; it != container.end(); ++it) {
    result += delimiter;
    result += *it;
  }

  return result;
}

inline std::string Replace(std::string str, std::string_view from,
                           std::string_view to) {
  if (from.empty()) return str;

  std::string::size_type pos = 0;
  while ((pos = str.find(from, pos)) != std::string::npos) {
    str.replace(pos, from.length(), to);
    pos += to.length();
  }

  return str;
}

inline std::string PadLeft(std::string str, std::size_t width,
                           char fill = ' ') {
  if (str.length() >= width) return str;
  str.insert(0, width - str.length(), fill);
  return str;
}

inline std::string PadRight(std::string str, std::size_t width,
                            char fill = ' ') {
  if (str.length() >= width) return str;
  str.append(width - str.length(), fill);
  return str;
}

inline bool IsEmpty(std::string_view str) { return str.empty(); }

inline bool IsBlank(std::string_view str) {
  return std::all_of(str.begin(), str.end(),
                     [](unsigned char c) { return std::isspace(c); });
}

inline bool IsNumeric(std::string_view str) {
  return !str.empty() &&
         std::all_of(str.begin(), str.end(),
                     [](unsigned char c) { return std::isdigit(c); });
}

inline bool IsAlpha(std::string_view str) {
  return !str.empty() &&
         std::all_of(str.begin(), str.end(),
                     [](unsigned char c) { return std::isalpha(c); });
}

inline bool IsAlphaNumeric(std::string_view str) {
  return !str.empty() &&
         std::all_of(str.begin(), str.end(),
                     [](unsigned char c) { return std::isalnum(c); });
}

}  // namespace ws
