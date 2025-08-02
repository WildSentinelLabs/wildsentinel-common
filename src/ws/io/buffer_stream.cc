#include "ws/io/buffer_stream.h"

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
  if (!EnsureNotClosed().Ok()) return 0;
  return length_ - origin_;
}

offset_t BufferStream::Position() {
  if (!EnsureNotClosed().Ok()) return 0;
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

Status BufferStream::SetPosition(offset_t value) {
  RETURN_IF_ERROR(EnsureNotClosed());
  if (value < 0)
    return Status(StatusCode::kBadRequest, "Negative position not allowed");

  if (value > length_ - origin_) STREAM_THROW_TOO_LONG();
  position_ = origin_ + value;
  return Status();
}

StatusOr<offset_t> BufferStream::Read(Span<unsigned char> buffer,
                                      offset_t offset, offset_t count) {
  RETURN_IF_ERROR(ValidateBufferArguments(buffer, offset, count));
  RETURN_IF_ERROR(EnsureNotClosed());
  offset_t n = length_ - position_;
  if (n > count) n = count;
  if (n <= 0) return 0;
  assert(n <= length_ - position_ && "n <= length_ - position_");
  std::memcpy(buffer + offset, buffer_ + position_, n);
  position_ += n;
  return n;
}

StatusOr<offset_t> BufferStream::Read(Span<unsigned char> buffer) {
  RETURN_IF_ERROR(EnsureNotClosed());
  offset_t n =
      std::min(static_cast<offset_t>(buffer.Length()), length_ - position_);
  if (n <= 0) return 0;
  std::memcpy(buffer, buffer_ + position_, n);
  position_ += n;
  return n;
}

StatusOr<int16_t> BufferStream::ReadByte() {
  RETURN_IF_ERROR(EnsureNotClosed());
  if (position_ >= length_) return -1;
  return static_cast<int16_t>(buffer_[static_cast<size_t>(position_++)]);
}

StatusOr<offset_t> BufferStream::Seek(offset_t offset, SeekOrigin origin) {
  RETURN_IF_ERROR(EnsureNotClosed());
  switch (origin) {
    case SeekOrigin::kBegin:
      if (offset < 0 || offset > length_ - origin_)
        return Status(StatusCode::kOutOfRange,
                      "IO Error: SeekBeforeBegin OutOfRange");
      position_ = offset;
      break;
    case SeekOrigin::kEnd:
      if (offset > 0 || offset < origin_ - length_)
        return Status(StatusCode::kOutOfRange,
                      "IO Error: SeekAfterEnd OutOfRange");
      position_ = length_ + offset;
      break;
    case SeekOrigin::kCurrent:
    default:
      if (offset > length_ - position_ || offset < origin_ - position_)
        return Status(StatusCode::kOutOfRange,
                      "IO Error: SeekCurrent OutOfRange");
      position_ = position_ + offset;
      break;
  }

  assert(position_ <= length_ && "position_ <= length_");
  return position_;
}

Status BufferStream::Write(ReadOnlySpan<unsigned char> buffer, offset_t offset,
                           offset_t count) {
  RETURN_IF_ERROR(ValidateBufferArguments(buffer, offset, count));
  RETURN_IF_ERROR(EnsureNotClosed());
  RETURN_IF_ERROR(EnsureWriteable());
  if (count > length_ - position_) STREAM_THROW_TOO_LONG();
  std::memcpy(buffer_ + position_, buffer + offset, count);
  position_ += count;
  return Status();
}

Status BufferStream::Write(ReadOnlySpan<unsigned char> buffer) {
  RETURN_IF_ERROR(EnsureNotClosed());
  RETURN_IF_ERROR(EnsureWriteable());
  if (buffer.Length() > length_ - position_) STREAM_THROW_TOO_LONG();
  std::memcpy(buffer_ + position_, buffer, buffer.Length());
  position_ += buffer.Length();
  return Status();
}

Status BufferStream::WriteByte(unsigned char value) {
  RETURN_IF_ERROR(EnsureNotClosed());
  RETURN_IF_ERROR(EnsureWriteable());
  if (position_ >= length_) STREAM_THROW_TOO_LONG();
  buffer_[static_cast<size_t>(position_++)] = value;
  return Status();
}

Status BufferStream::WriteTo(Stream& stream) {
  RETURN_IF_ERROR(EnsureNotClosed());
  RETURN_IF_ERROR(stream.Write(buffer_, origin_, length_ - origin_));
  return Status();
}

StatusOr<Array<unsigned char>> BufferStream::ToArray() {
  RETURN_IF_ERROR(EnsureNotClosed());
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

Status BufferStream::CopyTo(Stream& stream, offset_t buffer_size) {
  RETURN_IF_ERROR(ValidateCopyToArguments(stream, buffer_size));
  RETURN_IF_ERROR(EnsureNotClosed());
  offset_t original_pos = position_;
  offset_t remaining = Skip(length_ - original_pos);
  if (remaining > 0)
    RETURN_IF_ERROR(stream.Write(buffer_, position_, remaining));

  return Status();
}

Status BufferStream::EnsureNotClosed() const {
  if (!is_open_) STREAM_THROW_CLOSED();
  return Status();
}

Status BufferStream::EnsureWriteable() const {
  if (!CanWrite()) STREAM_THROW_UNWRITABLE();
  return Status();
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
