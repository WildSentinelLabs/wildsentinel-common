#pragma once

#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

#include "ws/machine.h"
#include "ws/status/status_or.h"

namespace ws {
namespace io {

class Directory {
 public:
  static StatusOr<std::vector<std::string>> GetFiles(const std::string& path);
  static StatusOr<std::vector<std::string>> GetDirectories(
      const std::string& path);
  static StatusOr<bool> Exists(const std::string& path);
  static StatusOr<void> Create(const std::string& path);
  static StatusOr<void> Delete(const std::string& path, bool recursive = false);

 private:
  static bool IsDirectory(const std::string& path);

  static bool IsRegularFile(const std::string& path);

  static constexpr int kDefaultPermissions = 0755;
};
}  // namespace io
}  // namespace ws
