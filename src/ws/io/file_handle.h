#pragma once

#define KEEP_WS_ORDER
#include "ws/system.h"
#undef KEEP_WS_ORDER

#ifdef _WIN32
#include <windows.h>
#else
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
#include <string>

#include "ws/io/file_access.h"
#include "ws/io/file_mode.h"
#include "ws/io/file_share.h"
#include "ws/io/seek_origin.h"
#include "ws/span.h"
#include "ws/status/status_or.h"
#include "ws/types.h"

namespace ws {
namespace io {
class FileHandle {
 public:
  static StatusOr<FileHandle> Open(const std::filesystem::path& full_path,
                                   FileMode mode, FileAccess access,
                                   FileShare share,
                                   offset_t preallocation_size = 0);

  FileHandle();

  ~FileHandle() = default;

  static Status SetFileLength(FileHandle& handle, offset_t length);
  static StatusOr<offset_t> ReadAtOffset(FileHandle& handle,
                                         Span<unsigned char> buffer,
                                         offset_t file_offset);
  static StatusOr<offset_t> Seek(FileHandle& handle, offset_t offset,
                                 SeekOrigin origin,
                                 bool close_invalid_handle = false);
  static Status WriteAtOffset(FileHandle& handle,
                              ReadOnlySpan<unsigned char> buffer,
                              offset_t file_offset);
  static StatusOr<bool> IsEndOfFile(offset_t error_code, FileHandle& handle,
                                    offset_t file_offset);

  std::filesystem::path Path() const;
  bool IsClosed() const;
  bool CanSeek();
  bool TryGetCachedLength(offset_t& cached_length);
  StatusOr<offset_t> FileType();
  StatusOr<offset_t> FileLength();
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
  offset_t length_;
  bool length_can_be_cached_;
  offset_t file_type_;
};

}  // namespace io
}  // namespace ws
