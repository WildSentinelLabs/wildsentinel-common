#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>

#include "ws/io/stream.h"
#include "ws/status/status_or.h"

namespace ws {
namespace io {

class BufferStream : public Stream {
 public:
  BufferStream(std::span<value_type> buffer);
  BufferStream(std::span<value_type> buffer, bool writable);
  BufferStream(std::span<value_type> buffer, bool writable, bool visible);

  ~BufferStream() = default;

  bool CanSeek() override;
  bool CanRead() const override;
  bool CanWrite() const override;
  size_type Length() override;
  size_type Position() override;
  bool TryGetBuffer(std::span<value_type>& buffer);
  bool TryGetBuffer(std::span<const value_type>& buffer) const;
  Status SetPosition(size_type value) override;
  StatusOr<size_type> Read(std::span<value_type> buffer, size_type offset,
                           size_type count) override;
  StatusOr<size_type> Read(std::span<value_type> buffer) override;
  StatusOr<int16_t> ReadByte() override;
  StatusOr<size_type> Seek(size_type offset, SeekOrigin origin) override;
  Status Write(std::span<const value_type> buffer, size_type offset,
               size_type count) override;
  Status Write(std::span<const value_type> buffer) override;
  Status WriteByte(value_type value) override;
  Status WriteTo(Stream& stream);
  StatusOr<container_type> ToArray() override;
  void Close() override;
  void Dispose() override;

 protected:
  Status CopyTo(Stream& stream, size_type buffer_size) override;

 private:
  Status EnsureNotClosed() const;
  Status EnsureWriteable() const;
  size_type Skip(size_type count);

  std::span<value_type> buffer_;
  size_type origin_;
  size_type position_;
  size_type length_;
  bool is_open_;
  bool writable_;
  bool exposable_;
};
}  // namespace io
}  // namespace ws
