#pragma once

#include <string>
#include <vector>

#include "ws/array.h"
#include "ws/io/file_access.h"
#include "ws/io/file_mode.h"
#include "ws/io/file_share.h"
#include "ws/io/file_stream.h"
#include "ws/span.h"
#include "ws/string/string_helpers.h"
#include "ws/types.h"

namespace ws {
namespace io {
class File {
 public:
  static StatusOr<FileStream> Create(const std::string& path);
  static StatusOr<FileStream> Open(const std::string& path);
  static StatusOr<FileStream> Open(const std::string& path, FileMode mode);
  static StatusOr<FileStream> Open(const std::string& path, FileMode mode,
                                   FileAccess access);
  static StatusOr<FileStream> Open(const std::string& path, FileMode mode,
                                   FileAccess access, FileShare share);
  static StatusOr<bool> Exists(const std::string& path);
  static Status Move(const std::string& sourcePath,
                     const std::string& destinationPath);
  static Status Delete(const std::string& path);
  static StatusOr<std::string> ReadAllText(const std::string& path);
  static StatusOr<Array<std::string>> ReadAllLines(const std::string& path);
  static StatusOr<Array<unsigned char>> ReadAllBytes(const std::string& path);

 private:
  static constexpr FileShare kDefaultShare = FileShare::kRead;
};
}  // namespace io
}  // namespace ws
