#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "ws/io/stream.h"
#include "ws/status/status_or.h"

namespace ws {
namespace io {

class BufferStream : public Stream {
 public:
  BufferStream(Span<unsigned char> buffer);
  BufferStream(Span<unsigned char> buffer, bool writable);
  BufferStream(Span<unsigned char> buffer, bool writable, bool visible);

  ~BufferStream() = default;

  bool CanSeek() override;
  bool CanRead() const override;
  bool CanWrite() const override;
  StatusOr<offset_t> Length() override;
  StatusOr<offset_t> Position() override;
  bool TryGetBuffer(Span<unsigned char>& buffer) const;
  Status SetPosition(offset_t value) override;
  StatusOr<offset_t> Read(Span<unsigned char> buffer, offset_t offset,
                          offset_t count) override;
  StatusOr<offset_t> Read(Span<unsigned char> buffer) override;
  StatusOr<int16_t> ReadByte() override;
  StatusOr<offset_t> Seek(offset_t offset, SeekOrigin origin) override;
  Status Write(ReadOnlySpan<unsigned char> buffer, offset_t offset,
               offset_t count) override;
  Status Write(ReadOnlySpan<unsigned char> buffer) override;
  Status WriteByte(unsigned char value) override;
  Status WriteTo(Stream& stream);
  StatusOr<Array<unsigned char>> ToArray() override;
  void Close() override;
  void Dispose() override;

 protected:
  Status CopyTo(Stream& stream, offset_t buffer_size) override;

 private:
  Status EnsureNotClosed() const;
  Status EnsureWriteable() const;
  offset_t Skip(offset_t count);

  Span<unsigned char> buffer_;
  offset_t origin_;
  offset_t position_;
  offset_t length_;
  bool is_open_;
  bool writable_;
  bool exposable_;
};
}  // namespace io
}  // namespace ws
