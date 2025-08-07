#include "ws/io/buffer_stream.h"

namespace ws {
namespace io {

BufferStream::BufferStream(std::span<value_type> buffer)
    : BufferStream(buffer, true) {}

BufferStream::BufferStream(std::span<value_type> buffer, bool writable)
    : BufferStream(buffer, writable, false) {}

BufferStream::BufferStream(std::span<value_type> buffer, bool writable,
                           bool visible)
    : buffer_(buffer),
      origin_(0),
      position_(0),
      length_(buffer.size()),
      is_open_(true),
      writable_(writable),
      exposable_(visible) {}

bool BufferStream::CanSeek() { return is_open_; }

bool BufferStream::CanRead() const { return is_open_; }

bool BufferStream::CanWrite() const { return writable_; }

BufferStream::size_type BufferStream::Length() {
  if (!EnsureNotClosed().Ok()) return 0;
  return length_ - origin_;
}

BufferStream::size_type BufferStream::Position() {
  if (!EnsureNotClosed().Ok()) return 0;
  return position_ - origin_;
}

bool BufferStream::TryGetBuffer(std::span<value_type>& buffer) {
  if (!exposable_) {
    buffer = std::span<value_type>();
    return false;
  }

  buffer = buffer_;
  return true;
}

bool BufferStream::TryGetBuffer(std::span<const value_type>& buffer) const {
  if (!exposable_) {
    buffer = std::span<const value_type>();
    return false;
  }

  buffer = buffer_;
  return true;
}

Status BufferStream::SetPosition(size_type value) {
  RETURN_IF_ERROR(EnsureNotClosed());
  if (value < 0)
    return Status(StatusCode::kBadRequest, "Negative position not allowed");

  if (value > length_ - origin_) STREAM_THROW_TOO_LONG();
  position_ = origin_ + value;
  return Status();
}

StatusOr<BufferStream::size_type> BufferStream::Read(
    std::span<value_type> buffer, size_type offset, size_type count) {
  RETURN_IF_ERROR(ValidateBufferArguments(buffer, offset, count));
  RETURN_IF_ERROR(EnsureNotClosed());
  size_type n = length_ - position_;
  if (n > count) n = count;
  if (n <= 0) return 0;
  assert(n <= length_ - position_ && "n <= length_ - position_");
  std::memcpy(buffer.data() + offset, buffer_.data() + position_, n);
  position_ += n;
  return n;
}

StatusOr<BufferStream::size_type> BufferStream::Read(
    std::span<value_type> buffer) {
  RETURN_IF_ERROR(EnsureNotClosed());
  size_type n =
      std::min(static_cast<size_type>(buffer.size()), length_ - position_);
  if (n <= 0) return 0;
  std::memcpy(buffer.data(), buffer_.data() + position_, n);
  position_ += n;
  return n;
}

StatusOr<int16_t> BufferStream::ReadByte() {
  RETURN_IF_ERROR(EnsureNotClosed());
  if (position_ >= length_) return -1;
  return static_cast<int16_t>(buffer_[static_cast<size_t>(position_++)]);
}

StatusOr<BufferStream::size_type> BufferStream::Seek(size_type offset,
                                                     SeekOrigin origin) {
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

Status BufferStream::Write(std::span<const value_type> buffer, size_type offset,
                           size_type count) {
  RETURN_IF_ERROR(ValidateBufferArguments(buffer, offset, count));
  RETURN_IF_ERROR(EnsureNotClosed());
  RETURN_IF_ERROR(EnsureWriteable());
  if (count > length_ - position_) STREAM_THROW_TOO_LONG();
  std::memcpy(buffer_.data() + position_, buffer.data() + offset, count);
  position_ += count;
  return Status();
}

Status BufferStream::Write(std::span<const value_type> buffer) {
  RETURN_IF_ERROR(EnsureNotClosed());
  RETURN_IF_ERROR(EnsureWriteable());
  if (buffer.size() > length_ - position_) STREAM_THROW_TOO_LONG();
  std::memcpy(buffer_.data() + position_, buffer.data(), buffer.size());
  position_ += buffer.size();
  return Status();
}

Status BufferStream::WriteByte(value_type value) {
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

StatusOr<BufferStream::container_type> BufferStream::ToArray() {
  RETURN_IF_ERROR(EnsureNotClosed());
  size_type count = length_ - origin_;
  if (count == 0) return container_type();
  container_type buffer(count);
  std::memcpy(buffer.data(), buffer_.data() + origin_, count);
  return buffer;
}

void BufferStream::Close() { Dispose(); }

void BufferStream::Dispose() {
  writable_ = false;
  is_open_ = false;
  exposable_ = false;
}

Status BufferStream::CopyTo(Stream& stream, size_type buffer_size) {
  RETURN_IF_ERROR(ValidateCopyToArguments(stream, buffer_size));
  RETURN_IF_ERROR(EnsureNotClosed());
  size_type original_pos = position_;
  size_type remaining = Skip(length_ - original_pos);
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

BufferStream::size_type BufferStream::Skip(size_type count) {
  size_type n = length_ - position_;
  if (n > count) n = count;
  if (n < 0) n = 0;
  assert(n <= kMaxLength - position_ && "n <= kMaxLength - position_");
  position_ += n;
  return n;
}

}  // namespace io
}  // namespace ws
