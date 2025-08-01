#pragma once

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <exception>
#include <memory>
#include <stdexcept>

#include "ws/io/file_access.h"
#include "ws/io/file_handle.h"
#include "ws/io/file_mode.h"
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
                                     offset_t buffer_size);
  static StatusOr<FileStream> Create(const std::string& path, FileMode mode,
                                     FileAccess access, FileShare share,
                                     offset_t buffer_size,
                                     offset_t preallocation_size);

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
  StatusOr<offset_t> Length() override;
  StatusOr<offset_t> Position() override;
  Status SetPosition(offset_t value) override;
  Status SetLength(offset_t value);
  StatusOr<offset_t> Read(Span<unsigned char> buffer, offset_t offset,
                          offset_t count) override;
  StatusOr<offset_t> Read(Span<unsigned char> buffer) override;
  StatusOr<int16_t> ReadByte() override;
  StatusOr<offset_t> Seek(offset_t offset, SeekOrigin origin) override;
  Status Write(ReadOnlySpan<unsigned char> buffer, offset_t offset,
               offset_t count) override;
  Status Write(ReadOnlySpan<unsigned char> buffer) override;
  Status WriteByte(unsigned char value) override;
  StatusOr<Array<unsigned char>> ToArray() override;
  void Close() override;
  void Dispose() override;

 private:
  static constexpr offset_t kDefaultBufferSize = 4096;

  FileHandle file_handle_;
  offset_t position_;
  offset_t append_start_;
  FileAccess access_;
};

}  // namespace io
}  // namespace ws
