#pragma once

#define KEEP_WS_ORDER
#include "ws/system.h"
#undef KEEP_WS_ORDER

#ifndef _WIN32
#define KEEP_LINUX_ORDER
#include <sys/types.h>
#undef KEEP_LINUX_ORDER
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#ifdef __APPLE__
#include <sys/fcntl.h>
#endif
#endif

#include <cerrno>
#include <cstring>
#include <filesystem>
#include <span>
#include <string>

#include "ws/io/file_access.h"
#include "ws/io/file_mode.h"
#include "ws/io/file_share.h"
#include "ws/io/seek_origin.h"
#include "ws/status/status_or.h"
#include "ws/types.h"

namespace ws {
namespace io {
class FileHandle {
 public:
  using value_type = unsigned char;
  using size_type = offset_t;

  static StatusOr<FileHandle> Open(const std::filesystem::path& full_path,
                                   FileMode mode, FileAccess access,
                                   FileShare share,
                                   size_type preallocation_size = 0);

  FileHandle();

  ~FileHandle() = default;

  static Status SetFileLength(FileHandle& handle, size_type length);
  static StatusOr<size_type> ReadAtOffset(FileHandle& handle,
                                          std::span<value_type> buffer,
                                          size_type file_offset);
  static StatusOr<size_type> Seek(FileHandle& handle, size_type offset,
                                  SeekOrigin origin,
                                  bool close_invalid_handle = false);
  static Status WriteAtOffset(FileHandle& handle,
                              std::span<const value_type> buffer,
                              size_type file_offset);
  static StatusOr<bool> IsEndOfFile(size_type error_code, FileHandle& handle,
                                    size_type file_offset);

  std::filesystem::path Path() const;
  bool IsClosed() const;
  bool CanSeek();
  bool TryGetCachedLength(size_type& cached_length);
  StatusOr<size_type> FileType();
  StatusOr<size_type> FileLength();
  void Dispose();

 private:
#ifdef _WIN32
  FileHandle(void* fd);
  void* fd_;
#else
  FileHandle(int fd);
  int fd_;
#endif

  std::filesystem::path path_;
  size_type length_;
  bool length_can_be_cached_;
  size_type file_type_;
};

}  // namespace io
}  // namespace ws
