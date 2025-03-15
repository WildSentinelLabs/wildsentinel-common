#pragma once
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>

#include "io/stream.h"
namespace ws {
namespace io {
namespace streams {
class MemoryStream : public Stream {
 public:
  explicit MemoryStream(size_t chunk_size);

  ~MemoryStream() override;

  bool CanRead() const override;

  bool CanWrite() const override;

  size_t GetLength() const override;

  size_t GetPosition() const override;

  void SetPosition(size_t offset) override;

  size_t Read(unsigned char buffer[], size_t offset, size_t count) override;

  bool Write(const unsigned char buffer[], size_t offset,
             size_t count) override;

  void Dispose() override;

 private:
  size_t position_;
  size_t chunk_index_;
  size_t chunk_offset_;
  unsigned char** memory_pool_;
  size_t chunk_size_;
  size_t total_chunks_;
  size_t used_size_;
  bool is_open_;
  bool is_writable_;
  std::atomic<bool> disposed_;

  void UpdateChunkPosition();

  void AllocateChunk();
};
}  // namespace streams
}  // namespace io
}  // namespace ws
