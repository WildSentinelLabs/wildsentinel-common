#pragma once

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <memory>
#include <stdexcept>

#include "io/file_access.h"
#include "io/file_handle.h"
#include "io/file_mode.h"
#include "io/stream.h"

namespace ws {
namespace io {

class FileStream : public Stream {
 public:
  FileStream();

  FileStream(const std::string& path);

  FileStream(const std::string& path, FileMode mode);

  FileStream(const std::string& path, FileMode mode, FileAccess access);

  FileStream(const std::string& path, FileMode mode, FileAccess access,
             FileShare share);

  FileStream(const std::string& path, FileMode mode, FileAccess access,
             FileShare share, offset_t buffer_size);

  ~FileStream() override;

  std::string Name() const;

  bool CanSeek() override;

  bool CanRead() const override;

  bool CanWrite() const override;

  offset_t Length() override;

  offset_t Position() override;

  void SetPosition(offset_t value) override;

  void SetLength(offset_t value);

  offset_t Read(Span<unsigned char> buffer, offset_t offset,
                offset_t count) override;

  offset_t Read(Span<unsigned char> buffer) override;

  int16_t ReadByte() override;

  offset_t Seek(offset_t offset, SeekOrigin origin) override;

  void Write(ReadOnlySpan<unsigned char> buffer, offset_t offset,
             offset_t count) override;

  void Write(ReadOnlySpan<unsigned char> buffer) override;

  void WriteByte(unsigned char value) override;

  Array<unsigned char> ToArray() override;

  void Close() override;

  void Dispose() override;

 private:
  static constexpr offset_t kDefaultBufferSize = 4096;
  static constexpr FileShare kDefaultShare = FileShare::kRead;

  FileStream(const std::string& path, FileMode mode, FileAccess access,
             FileShare share, offset_t buffer_size,
             offset_t preallocation_size);

  FileHandle file_handle_;
  offset_t position_;
  offset_t append_start_;
  FileAccess access_;
};

}  // namespace io
}  // namespace ws
