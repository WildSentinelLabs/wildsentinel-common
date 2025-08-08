#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "ws/io/stream.h"
#include "ws/status/status_or.h"

namespace ws {
namespace io {
class MemoryStream : public Stream {
 public:
  MemoryStream();
  MemoryStream(size_type capacity);
  MemoryStream(size_type capacity, bool visible);
  MemoryStream(container_type&& buffer);
  MemoryStream(container_type&& buffer, bool writable);
  MemoryStream(container_type&& buffer, bool writable, bool visible);
  MemoryStream(MemoryStream&&) noexcept;
  MemoryStream(const MemoryStream&) = delete;

  MemoryStream& operator=(const MemoryStream&) = delete;
  MemoryStream& operator=(MemoryStream&&) noexcept;

  ~MemoryStream() = default;

  bool CanSeek() override;
  bool CanRead() const override;
  bool CanWrite() const override;
  size_type Length() override;
  size_type Position() override;
  size_type Capacity() const;
  bool TryGetBuffer(std::span<value_type>& buffer);
  bool TryGetBuffer(std::span<const value_type>& buffer) const;
  Status SetPosition(size_type value) override;
  Status SetLength(size_type value);
  Status SetCapacity(size_type value);
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

 protected:
  Status CopyTo(Stream& stream, size_type buffer_size) override;

 private:
  Status EnsureNotClosed() const;
  Status EnsureWriteable() const;
  StatusOr<bool> EnsureCapacity(size_type value);
  size_type Skip(size_type count);

  static constexpr const size_type k256 = 256;

  container_type buffer_;
  size_type position_;
  size_type length_;
  size_type capacity_;
  bool expandable_;
  bool writable_;
  bool exposable_;
  bool is_open_;
};
}  // namespace io
}  // namespace ws
