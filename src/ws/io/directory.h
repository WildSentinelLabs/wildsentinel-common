#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#define KEEP_LINUX_ORDER
#include <sys/types.h>
#undef KEEP_LINUX_ORDER
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <algorithm>
#include <string>
#include <vector>

#include "ws/io/path.h"
#include "ws/status/status_or.h"

namespace ws {
namespace io {

class Directory {
 public:
  static StatusOr<std::vector<std::string>> GetFiles(const std::string& path);
  static StatusOr<std::vector<std::string>> GetDirectories(
      const std::string& path);
  static StatusOr<bool> Exists(const std::string& path);
  static Status Create(const std::string& path);
  static Status Delete(const std::string& path, bool recursive = false);

 private:
  static constexpr int kDefaultPermissions = 0755;
};
}  // namespace io
}  // namespace ws
