#pragma once
#include <filesystem>
#include <stdexcept>
#include <string>

#include "idisposable.h"
#include "io/file_access.h"
#include "io/file_mode.h"
#include "io/file_share.h"
#include "io/seek_origin.h"
#include "span.h"
#include "types.h"

namespace ws {
namespace io {
class FileHandle : public IDisposable {
 public:
  explicit FileHandle();

  static FileHandle Open(const std::filesystem::path& full_path, FileMode mode,
                         FileAccess access, FileShare share,
                         offset_t preallocation_size = 0);

  static void SetFileLength(FileHandle& handle, offset_t length);

  static offset_t ReadAtOffset(FileHandle& handle, Span<unsigned char> buffer,
                               offset_t file_offset);

  static offset_t Seek(FileHandle& handle, offset_t offset, SeekOrigin origin,
                       bool close_invalid_handle = false);

  static void WriteAtOffset(FileHandle& handle,
                            ReadOnlySpan<unsigned char> buffer,
                            offset_t file_offset);

  static bool IsEndOfFile(offset_t error_code, FileHandle& handle,
                          offset_t file_offset);

  static std::filesystem::path GetFullPath(const std::string& input);

  static std::filesystem::path GetFullPath(const std::wstring& input);

  std::filesystem::path Path() const;

  bool IsClosed() const;

  bool CanSeek();

  bool TryGetCachedLength(offset_t& cached_length);

  void* Get();

  offset_t FileType();

  offset_t FileLength();

  void Dispose() override;

 private:
  explicit FileHandle(void* handle);

  void* handle_;
  std::filesystem::path path_;
  offset_t length_;
  bool length_can_be_cached_;
  offset_t file_type_;
};

}  // namespace io
}  // namespace ws
