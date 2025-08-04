#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>

#include "ws/io/stream.h"
#include "ws/status/status_or.h"

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
  MemoryStream(MemoryStream&&) noexcept;
  MemoryStream(const MemoryStream&) = delete;

  MemoryStream& operator=(const MemoryStream&) = delete;
  MemoryStream& operator=(MemoryStream&&) noexcept;

  ~MemoryStream() = default;

  bool CanSeek() override;
  bool CanRead() const override;
  bool CanWrite() const override;
  offset_t Length() override;
  offset_t Position() override;
  offset_t Capacity() const;
  bool TryGetBuffer(Span<unsigned char>& buffer) const;
  Status SetPosition(offset_t value) override;
  Status SetLength(offset_t value);
  Status SetCapacity(offset_t value);
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

 protected:
  Status CopyTo(Stream& stream, offset_t buffer_size) override;

 private:
  Status EnsureNotClosed() const;
  Status EnsureWriteable() const;
  StatusOr<bool> EnsureCapacity(offset_t value);
  offset_t Skip(offset_t count);

  static constexpr const offset_t k256 = 256;

  Array<unsigned char> buffer_;
  offset_t position_;
  offset_t length_;
  offset_t capacity_;
  bool expandable_;
  bool writable_;
  bool exposable_;
  bool is_open_;
};
}  // namespace io
}  // namespace ws
