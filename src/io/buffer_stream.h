#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "io/stream.h"

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
  offset_t Length() override;
  offset_t Position() override;
  bool TryGetBuffer(Span<unsigned char>& buffer) const;
  void SetPosition(offset_t value) override;
  offset_t Read(Span<unsigned char> buffer, offset_t offset,
                offset_t count) override;
  offset_t Read(Span<unsigned char> buffer) override;
  int16_t ReadByte() override;
  offset_t Seek(offset_t offset, SeekOrigin origin) override;
  void Write(ReadOnlySpan<unsigned char> buffer, offset_t offset,
             offset_t count) override;
  void Write(ReadOnlySpan<unsigned char> buffer) override;
  void WriteByte(unsigned char value) override;
  void WriteTo(Stream& stream);
  Array<unsigned char> ToArray() override;
  void Close() override;
  void Dispose() override;

 protected:
  void CopyTo(Stream& stream, offset_t buffer_size) override;

 private:
  void EnsureNotClosed() const;
  void EnsureWriteable() const;
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
