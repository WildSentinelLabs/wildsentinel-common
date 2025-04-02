#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>

#include "io/stream.h"
namespace ws {
namespace io {
class MemoryStream : public Stream {
 public:
  MemoryStream();

  MemoryStream(offset_t capacity);

  MemoryStream(offset_t capacity, bool visible);

  MemoryStream(Array<unsigned char>&& buffer);

  MemoryStream(Array<unsigned char>&& buffer, bool writable);

  MemoryStream(Array<unsigned char>&& buffer, bool writable, bool visible);

  MemoryStream(MemoryStream&& other) noexcept;

  MemoryStream(const MemoryStream&) = delete;

  ~MemoryStream() override;

  bool CanSeek() override;

  bool CanRead() const override;

  bool CanWrite() const override;

  offset_t Length() override;

  offset_t Position() override;

  offset_t Capacity() const;

  bool TryGetBuffer(Span<unsigned char>& buffer) const;

  void SetPosition(offset_t value) override;

  void SetLength(offset_t value);

  void SetCapacity(offset_t value);

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

  MemoryStream& operator=(const MemoryStream&) = delete;

  MemoryStream& operator=(MemoryStream&& other) noexcept;

 protected:
  void CopyTo(Stream& stream, offset_t buffer_size) override;

 private:
  Array<unsigned char> buffer_;
  offset_t position_;
  offset_t length_;
  offset_t capacity_;
  bool expandable_;
  bool writable_;
  bool exposable_;
  bool is_open_;

  void EnsureNotClosed() const;

  void EnsureWriteable() const;

  bool EnsureCapacity(offset_t value);

  offset_t Skip(offset_t count);
};
}  // namespace io
}  // namespace ws
