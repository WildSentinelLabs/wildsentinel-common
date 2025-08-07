#pragma once

#define KEEP_WS_ORDER
#include "ws/system.h"
#undef KEEP_WS_ORDER

#ifdef _WIN32
#include <windows.h>
#else
#include <errno.h>
#include <sys/stat.h>
#endif

#include <filesystem>
#include <string>

#include "ws/status/status_or.h"

namespace ws {
namespace io {

class Path {
 public:
  static StatusOr<bool> IsFile(const std::string& path);
  static StatusOr<bool> IsDirectory(const std::string& path);
  static StatusOr<bool> IsSpecialFile(const std::string& path);
  static StatusOr<bool> IsSymbolicLink(const std::string& path);
  static std::string NormalizePath(const std::string& path);
  static StatusOr<std::string> GetParent(const std::string& path);
  static StatusOr<std::string> GetExtension(const std::string& path);
  static std::filesystem::path GetFullPath(const std::string& input);
  static std::filesystem::path GetFullPath(const std::wstring& input);

 private:
  static constexpr int kDefaultPermissions = 0755;
};

}  // namespace io
}  // namespace ws
