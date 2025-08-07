#pragma once

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <memory>
#include <span>
#include <type_traits>

#include "ws/io/file_access.h"
#include "ws/io/file_handle.h"
#include "ws/io/file_mode.h"
#include "ws/io/path.h"
#include "ws/io/stream.h"
#include "ws/status/status_or.h"

namespace ws {
namespace io {

class FileStream : public Stream {
 public:
  static StatusOr<FileStream> Create(const std::string& path, FileMode mode,
                                     FileAccess access, FileShare share);
  static StatusOr<FileStream> Create(const std::string& path, FileMode mode,
                                     FileAccess access, FileShare share,
                                     size_type buffer_size);
  static StatusOr<FileStream> Create(const std::string& path, FileMode mode,
                                     FileAccess access, FileShare share,
                                     size_type buffer_size,
                                     size_type preallocation_size);

  FileStream();

  FileStream(FileStream&&) noexcept;
  FileStream(const FileStream&) = delete;

  FileStream& operator=(const FileStream&) = delete;
  FileStream& operator=(FileStream&&) noexcept;

  ~FileStream() override;

  std::string Name() const;
  bool CanSeek() override;
  bool CanRead() const override;
  bool CanWrite() const override;
  size_type Length() override;
  size_type Position() override;
  Status SetPosition(size_type value) override;
  Status SetLength(size_type value);
  StatusOr<size_type> Read(std::span<value_type> buffer, size_type offset,
                           size_type count) override;
  StatusOr<size_type> Read(std::span<value_type> buffer) override;
  StatusOr<int16_t> ReadByte() override;
  StatusOr<size_type> Seek(size_type offset, SeekOrigin origin) override;
  Status Write(std::span<const value_type> buffer, size_type offset,
               size_type count) override;
  Status Write(std::span<const value_type> buffer) override;
  Status WriteByte(value_type value) override;
  StatusOr<container_type> ToArray() override;
  void Close() override;
  void Dispose() override;

 private:
  FileStream(FileHandle file_handle, size_type position, size_type append_start,
             FileAccess access);

  static constexpr size_type kDefaultBufferSize = 4096;

  static_assert(std::is_same_v<FileHandle::value_type, Stream::value_type>,
                "FileHandle::value_type must match Stream::value_type");
  static_assert(std::is_same_v<FileHandle::size_type, Stream::size_type>,
                "FileHandle::size_type must match Stream::size_type");

  FileHandle file_handle_;
  size_type position_;
  size_type append_start_;
  FileAccess access_;
};

}  // namespace io
}  // namespace ws
