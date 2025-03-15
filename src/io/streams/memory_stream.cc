#include "io/streams/memory_stream.h"
namespace ws {
namespace io {
namespace streams {

void MemoryStream::UpdateChunkPosition() {
  chunk_index_ = position_ / chunk_size_;
  chunk_offset_ = position_ % chunk_size_;
}

void MemoryStream::AllocateChunk() {
  uint8_t** new_memory_pool = new uint8_t*[total_chunks_ + 1];
  for (size_t i = 0; i < total_chunks_; ++i) {
    new_memory_pool[i] = memory_pool_[i];
  }
  delete[] memory_pool_;
  memory_pool_ = new_memory_pool;
  memory_pool_[total_chunks_] = new uint8_t[chunk_size_];
  ++total_chunks_;
}

MemoryStream::MemoryStream(size_t chunk_size)
    : chunk_size_(chunk_size),
      position_(0),
      chunk_index_(0),
      chunk_offset_(0),
      memory_pool_(nullptr),
      total_chunks_(0),
      used_size_(0),
      is_open_(true),
      is_writable_(true) {
  if (chunk_size_ == 0) {
    throw std::invalid_argument("Chunk size must be greater than zero.");
  }

  AllocateChunk();
}

MemoryStream::~MemoryStream() { Dispose(); }

bool MemoryStream::CanRead() const { return is_open_; }

bool MemoryStream::CanWrite() const { return is_writable_; }

size_t MemoryStream::GetLength() const { return used_size_; }

size_t MemoryStream::GetPosition() const { return position_; }

void MemoryStream::SetPosition(size_t offset) {
  if (offset > used_size_) {
    throw std::out_of_range("Offset exceeds the used size.");
  }

  position_ = offset;
  UpdateChunkPosition();
}

size_t MemoryStream::Read(uint8_t buffer[], size_t offset, size_t count) {
  if (!is_open_ || position_ >= used_size_) {
    return 0;
  }

  size_t bytes_to_read = std::min(count, used_size_ - position_);
  size_t bytes_read = 0;
  while (bytes_read < bytes_to_read) {
    size_t bytes_in_chunk =
        std::min(chunk_size_ - chunk_offset_, bytes_to_read - bytes_read);

    std::memcpy(&buffer[offset + bytes_read],
                &memory_pool_[chunk_index_][chunk_offset_], bytes_in_chunk);

    bytes_read += bytes_in_chunk;
    position_ += bytes_in_chunk;
    chunk_offset_ += bytes_in_chunk;

    if (chunk_offset_ >= chunk_size_) {
      ++chunk_index_;
      chunk_offset_ = 0;
    }
  }

  return bytes_read;
}

bool MemoryStream::Write(const uint8_t buffer[], size_t offset, size_t count) {
  if (!is_writable_) {
    return false;
  }

  size_t bytes_written = 0;
  while (bytes_written < count) {
    if (position_ >= total_chunks_ * chunk_size_) {
      AllocateChunk();
    }

    size_t bytes_in_chunk =
        std::min(chunk_size_ - chunk_offset_, count - bytes_written);

    std::memcpy(&memory_pool_[chunk_index_][chunk_offset_],
                &buffer[offset + bytes_written], bytes_in_chunk);

    bytes_written += bytes_in_chunk;
    position_ += bytes_in_chunk;
    chunk_offset_ += bytes_in_chunk;

    if (chunk_offset_ >= chunk_size_) {
      ++chunk_index_;
      chunk_offset_ = 0;
    }
  }

  used_size_ = std::max(used_size_, position_);
  return true;
}

void MemoryStream::Dispose() {
  if (memory_pool_) {
    for (size_t i = 0; i < total_chunks_; ++i) {
      delete[] memory_pool_[i];
    }
    delete[] memory_pool_;
    memory_pool_ = nullptr;
  }

  position_ = 0;
  chunk_index_ = 0;
  chunk_offset_ = 0;
  total_chunks_ = 0;
  used_size_ = 0;
  is_open_ = false;
  is_writable_ = false;
}
}  // namespace streams
}  // namespace io
}  // namespace ws
// TODO: USE DISPOSED_
