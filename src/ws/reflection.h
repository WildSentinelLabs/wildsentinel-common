#pragma once

#include <string>

template <typename T>
inline std::string ExtractClassName() {
  constexpr const char* full_name =
#ifdef _MSC_VER
      __FUNCSIG__;
#else
      __PRETTY_FUNCTION__;
#endif
  std::string name = full_name;

#ifdef _MSC_VER
  size_t start = name.find('<') + 1;
  size_t end = name.find_last_of('>');
  if (start != std::string::npos && end != std::string::npos && start < end)
    name = name.substr(start, end - start);
#else
  size_t with_pos = name.find("with T = ");
  if (with_pos != std::string::npos) {
    size_t start = with_pos + 9;
    size_t end = name.find(';', start);
    if (end == std::string::npos) end = name.find(']', start);
    if (end != std::string::npos) name = name.substr(start, end - start);
  }
#endif

  size_t last_colon = name.find_last_of(':');
  if (last_colon != std::string::npos && last_colon + 1 < name.length())
    name = name.substr(last_colon + 1);

  return name;
}

#define EXTRACT_CLASS_NAME(T) ExtractClassName<T>()
