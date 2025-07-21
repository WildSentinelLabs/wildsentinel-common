#pragma once

#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>
#include <vector>

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

inline std::vector<std::string> Split(std::string_view str, char delimiter) {
  std::vector<std::string> tokens;
  std::string::size_type start = 0;
  std::string::size_type end = str.find(delimiter);

  while (end != std::string::npos) {
    tokens.emplace_back(str.substr(start, end - start));
    start = end + 1;
    end = str.find(delimiter, start);
  }

  tokens.emplace_back(str.substr(start));
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
