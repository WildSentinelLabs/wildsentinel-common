#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "io/streams/stream.h"

class BufferStream : public Stream {
 public:
  BufferStream(unsigned char* buffer, const size_t size,
               const bool is_writable);

  ~BufferStream() override;

  bool CanRead() const override;

  bool CanWrite() const override;

  size_t GetLength() const override;

  size_t GetPosition() const override;

  void SetPosition(size_t offset) override;

  size_t Read(unsigned char buffer[], size_t offset, size_t count) override;

  size_t Read(unsigned char buffer[], size_t count) override;

  bool Write(const unsigned char buffer[], size_t offset,
             size_t count) override;

  bool WriteTo(Stream& stream) override;

  unsigned char* ToArray() override;

  void Dispose() override;

 private:
  size_t position_;
  unsigned char* buffer_;
  size_t size_;
  bool is_open_;
  bool is_writable_;
};
