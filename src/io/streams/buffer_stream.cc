#include "io/streams/buffer_stream.h"

namespace ws {
namespace io {
namespace streams {

BufferStream::BufferStream(unsigned char* buffer, const size_t size,
                           const bool is_writable)
    : buffer_(buffer),
      size_(size),
      position_(0),
      is_writable_(is_writable),
      is_open_(true) {}

BufferStream::~BufferStream() { Dispose(); }

bool BufferStream::CanRead() const { return is_open_; }

bool BufferStream::CanWrite() const { return is_writable_; }

size_t BufferStream::GetLength() const { return size_; }

size_t BufferStream::GetPosition() const { return position_; }

void BufferStream::SetPosition(size_t offset) {
  if (offset <= size_) position_ = offset;
}

size_t BufferStream::Read(unsigned char buffer[], size_t offset, size_t count) {
  if (position_ >= size_) return 0;
  size_t available = std::min(count, size_ - position_);
  std::memcpy(buffer + offset, buffer_ + position_, available);
  position_ += available;
  return available;
}

size_t BufferStream::Read(unsigned char buffer[], size_t count) {
  return Read(buffer, 0, count);
}

bool BufferStream::Write(const unsigned char buffer[], size_t offset,
                         size_t count) {
  if (!is_writable_ || position_ >= size_) return false;
  size_t available = std::min(count, size_ - position_);
  std::memcpy(buffer_ + position_, buffer + offset, available);
  position_ += available;
  return available == count;
}

bool BufferStream::WriteTo(Stream& stream) {
  if (size_ == 0) return false;
  size_t copied = stream.Write(buffer_, 0, size_);
  return copied == size_;
}

unsigned char* BufferStream::ToArray() {
  unsigned char* arrayCopy = new unsigned char[size_];
  std::memcpy(arrayCopy, buffer_, size_);
  return arrayCopy;
}

void BufferStream::Dispose() {
  if (buffer_) {
    // delete[] buffer;
    buffer_ = nullptr;
  }

  size_ = 0;
  position_ = 0;
  is_writable_ = false;
  is_open_ = false;
}
}  // namespace streams
}  // namespace io
}  // namespace ws
// TODO: USE DISPOSED_
