#include "io/buffer_stream.h"

namespace ws {
namespace io {

BufferStream::BufferStream(Span<unsigned char> buffer)
    : BufferStream(buffer, true) {}

BufferStream::BufferStream(Span<unsigned char> buffer, bool writable)
    : BufferStream(buffer, writable, false) {}

BufferStream::BufferStream(Span<unsigned char> buffer, bool writable,
                           bool visible)
    : buffer_(buffer),
      origin_(0),
      position_(0),
      length_(buffer.Length()),
      is_open_(true),
      writable_(writable),
      exposable_(visible) {}

bool BufferStream::CanSeek() { return is_open_; }

bool BufferStream::CanRead() const { return is_open_; }

bool BufferStream::CanWrite() const { return writable_; }

offset_t BufferStream::Length() {
  EnsureNotClosed();
  return length_ - origin_;
}

offset_t BufferStream::Position() {
  EnsureNotClosed();
  return position_ - origin_;
}

bool BufferStream::TryGetBuffer(Span<unsigned char>& buffer) const {
  if (!exposable_) {
    buffer = Span<unsigned char>();
    return false;
  }

  buffer = Span<unsigned char>(buffer_, origin_, length_ - origin_);
  return true;
}

void BufferStream::SetPosition(offset_t value) {
  EnsureNotClosed();
  if (value < 0)
    WsException::UnderflowError("Negative position not allowed").Throw();
  if (value > length_ - origin_) ThrowStreamTooLong();
  position_ = origin_ + value;
}

offset_t BufferStream::Read(Span<unsigned char> buffer, offset_t offset,
                            offset_t count) {
  ValidateBufferArguments(buffer, offset, count);
  EnsureNotClosed();
  offset_t n = length_ - position_;
  if (n > count) n = count;
  if (n <= 0) return 0;
  assert(n <= length_ - position_ && "n <= length_ - position_");
  std::memcpy(buffer + offset, buffer_ + position_, n);
  position_ += n;
  return n;
}

offset_t BufferStream::Read(Span<unsigned char> buffer) {
  EnsureNotClosed();
  offset_t n =
      std::min(static_cast<offset_t>(buffer.Length()), length_ - position_);
  if (n <= 0) return 0;
  std::memcpy(buffer, buffer_ + position_, n);
  position_ += n;
  return n;
}

int16_t BufferStream::ReadByte() {
  EnsureNotClosed();
  if (position_ >= length_) return -1;
  return static_cast<int16_t>(buffer_[static_cast<size_t>(position_++)]);
}

offset_t BufferStream::Seek(offset_t offset, SeekOrigin origin) {
  EnsureNotClosed();
  switch (origin) {
    case SeekOrigin::kBegin:
      if (offset < 0 || offset > length_ - origin_)
        WsException::IOError("IO SeekBeforeBegin OutOfRange").Throw();
      position_ = offset;
      break;
    case SeekOrigin::kEnd:
      if (offset > 0 || offset < origin_ - length_)
        WsException::IOError("IO SeekBeforeEnd OutOfRange").Throw();
      position_ = length_ + offset;
      break;
    case SeekOrigin::kCurrent:
    default:
      if (offset > length_ - position_ || offset < origin_ - position_)
        WsException::IOError("IO SeekBeforeCurrent OutOfRange").Throw();
      position_ = position_ + offset;
      break;
  }

  assert(position_ <= length_ && "position_ <= length_");
  return position_;
}

void BufferStream::Write(ReadOnlySpan<unsigned char> buffer, offset_t offset,
                         offset_t count) {
  ValidateBufferArguments(buffer, offset, count);
  EnsureNotClosed();
  EnsureWriteable();
  if (count > length_ - position_) ThrowStreamTooLong();
  std::memcpy(buffer_ + position_, buffer + offset, count);
  position_ += count;
}

void BufferStream::Write(ReadOnlySpan<unsigned char> buffer) {
  EnsureNotClosed();
  EnsureWriteable();
  if (buffer.Length() > length_ - position_) ThrowStreamTooLong();
  std::memcpy(buffer_ + position_, buffer, buffer.Length());
  position_ += buffer.Length();
}

void BufferStream::WriteByte(unsigned char value) {
  EnsureNotClosed();
  EnsureWriteable();
  if (position_ >= length_) ThrowStreamTooLong();
  buffer_[static_cast<size_t>(position_++)] = value;
}

void BufferStream::WriteTo(Stream& stream) {
  EnsureNotClosed();
  stream.Write(buffer_, origin_, length_ - origin_);
}

Array<unsigned char> BufferStream::ToArray() {
  EnsureNotClosed();
  offset_t count = length_ - origin_;
  if (count == 0) return Array<unsigned char>();
  Array<unsigned char> array(count);
  std::memcpy(array, buffer_ + origin_, count);
  return array;
}

void BufferStream::Close() { Dispose(); }

void BufferStream::Dispose() {
  writable_ = false;
  is_open_ = false;
  exposable_ = false;
}

void BufferStream::CopyTo(Stream& stream, offset_t buffer_size) {
  ValidateCopyToArguments(stream, buffer_size);
  EnsureNotClosed();
  offset_t original_pos = position_;
  offset_t remaining = Skip(length_ - original_pos);
  if (remaining > 0) stream.Write(buffer_, position_, remaining);
}

void BufferStream::EnsureNotClosed() const {
  if (!is_open_) ThrowStreamClosed;
}

void BufferStream::EnsureWriteable() const {
  if (!CanWrite()) ThrowUnwritableStream();
}

offset_t BufferStream::Skip(offset_t count) {
  offset_t n = length_ - position_;
  if (n > count) n = count;
  if (n < 0) n = 0;
  assert(n <= kMaxLength - position_ && "n <= kMaxLength - position_");
  position_ += n;
  return n;
}

}  // namespace io
}  // namespace ws
