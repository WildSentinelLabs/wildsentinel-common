#include "ws/io/memory_stream.h"
namespace ws {
namespace io {

MemoryStream::MemoryStream() : MemoryStream(0) {}

MemoryStream::MemoryStream(offset_t capacity)
    : buffer_(capacity > 0 ? std::move(Array<unsigned char>(capacity))
                           : Array<unsigned char>()),
      position_(0),
      length_(capacity),
      capacity_(length_),
      expandable_(true),
      writable_(true),
      exposable_(true),
      is_open_(true) {}

MemoryStream::MemoryStream(Array<unsigned char>&& buffer)
    : MemoryStream(std::move(buffer), true) {}

MemoryStream::MemoryStream(Array<unsigned char>&& buffer, bool writable)
    : MemoryStream(std::move(buffer), writable, false) {}

MemoryStream::MemoryStream(Array<unsigned char>&& buffer, bool writable,
                           bool visible)
    : buffer_(std::move(buffer)),
      position_(0),
      length_(buffer_.Length()),
      capacity_(length_),
      expandable_(false),
      writable_(writable),
      exposable_(visible),
      is_open_(true) {}

MemoryStream::MemoryStream(MemoryStream&& other) noexcept
    : buffer_(std::move(other.buffer_)),
      position_(other.position_),
      length_(other.length_),
      capacity_(other.capacity_),
      expandable_(other.expandable_),
      writable_(other.writable_),
      exposable_(other.exposable_),
      is_open_(other.is_open_) {
  other.writable_ = false;
  other.is_open_ = false;
  other.exposable_ = false;
}

bool MemoryStream::CanSeek() { return is_open_; }

bool MemoryStream::CanRead() const { return is_open_; }

bool MemoryStream::CanWrite() const { return writable_; }

offset_t MemoryStream::Length() {
  if (!EnsureNotClosed().Ok()) return 0;
  return length_;
}

offset_t MemoryStream::Position() {
  if (!EnsureNotClosed().Ok()) return 0;
  return position_;
}

offset_t MemoryStream::Capacity() const {
  if (!EnsureNotClosed().Ok()) return 0;
  return capacity_;
}

bool MemoryStream::TryGetBuffer(Span<unsigned char>& buffer) const {
  if (!exposable_) {
    buffer = Span<unsigned char>();
    return false;
  }

  buffer = buffer_;
  return true;
}

Status MemoryStream::SetPosition(offset_t value) {
  if (value < 0)
    return Status(StatusCode::kBadRequest, "Negative position not allowed");

  RETURN_IF_ERROR(EnsureNotClosed());
  if (value > kMaxLength) STREAM_THROW_TOO_LONG();
  position_ = value;
  return Status();
}

Status MemoryStream::SetLength(offset_t value) {
  if (value < 0)
    return Status(StatusCode::kBadRequest, "Negative length not allowed");

  if (value > kMaxLength) STREAM_THROW_TOO_LONG();
  RETURN_IF_ERROR(EnsureWriteable());
  bool ensure_capacity;
  ASSIGN_OR_RETURN(ensure_capacity, EnsureCapacity(value));
  if (!ensure_capacity && value > length_)
    std::memset(buffer_ + value - length_, 0, length_);

  length_ = value;
  if (position_ > value) position_ = value;
  return Status();
}

Status MemoryStream::SetCapacity(offset_t value) {
  if (value < length_)
    return Status(StatusCode::kBadRequest, "Small capacity out of range");

  RETURN_IF_ERROR(EnsureNotClosed());
  offset_t current_capacity = capacity_;
  if (!expandable_ && (value != current_capacity))
    STREAM_THROW_NOT_EXPANDABLE();
  else if (expandable_ && value != capacity_) {
    if (value > 0) {
      Array<unsigned char> new_buffer(value);
      if (length_ > 0) std::memcpy(new_buffer, buffer_, length_);
      buffer_ = std::move(new_buffer);
    } else {
      buffer_ = std::move(Array<unsigned char>());
    }

    capacity_ = value;
  }

  return Status();
}

StatusOr<offset_t> MemoryStream::Read(Span<unsigned char> buffer,
                                      offset_t offset, offset_t count) {
  RETURN_IF_ERROR(ValidateBufferArguments(buffer, offset, count));
  RETURN_IF_ERROR(EnsureNotClosed());
  offset_t n = length_ - position_;
  if (n > count) n = count;
  if (n <= 0) return 0;
  assert(n <= kMaxLength - position_ && "n <= kMaxLength - position_");
  std::memcpy(buffer + offset, buffer_ + position_, n);
  position_ += n;
  return n;
}

StatusOr<offset_t> MemoryStream::Read(Span<unsigned char> buffer) {
  RETURN_IF_ERROR(EnsureNotClosed());
  offset_t n =
      std::min(static_cast<offset_t>(buffer.Length()), length_ - position_);
  if (n <= 0) return 0;
  std::memcpy(buffer, buffer_ + position_, n);
  position_ += n;
  return n;
}

StatusOr<int16_t> MemoryStream::ReadByte() {
  RETURN_IF_ERROR(EnsureNotClosed());
  if (position_ >= length_) return -1;
  return static_cast<int16_t>(buffer_[static_cast<size_t>(position_++)]);
}

StatusOr<offset_t> MemoryStream::Seek(offset_t offset, SeekOrigin origin) {
  RETURN_IF_ERROR(EnsureNotClosed());
  switch (origin) {
    case SeekOrigin::kBegin:
      if (offset < 0 || offset > kMaxLength)
        return Status(StatusCode::kOutOfRange,
                      "IO Error: SeekBeforeBegin OutOfRange");
      position_ = offset;
      break;
    case SeekOrigin::kEnd:
      if (offset > kMaxLength - length_ || offset > -length_)
        return Status(StatusCode::kOutOfRange,
                      "IO Error: SeekAfterEnd OutOfRange");
      position_ = length_ + offset;
      break;
    case SeekOrigin::kCurrent:
    default:
      if (offset > kMaxLength - position_ || offset > -position_)
        return Status(StatusCode::kOutOfRange,
                      "IO Error: SeekCurrent OutOfRange");
      position_ = position_ + offset;
      break;
  }

  assert(position_ <= kMaxLength && "position_ <= kMaxLength");
  return position_;
}

Status MemoryStream::Write(ReadOnlySpan<unsigned char> buffer, offset_t offset,
                           offset_t count) {
  RETURN_IF_ERROR(ValidateBufferArguments(buffer, offset, count));
  RETURN_IF_ERROR(EnsureNotClosed());
  RETURN_IF_ERROR(EnsureWriteable());
  if (count > kMaxLength - position_) STREAM_THROW_TOO_LONG();
  offset_t i = position_ + count;
  if (i > length_) {
    bool must_zero = position_ > length_;
    if (i > capacity_) {
      bool ensure_capacity;
      ASSIGN_OR_RETURN(ensure_capacity, EnsureCapacity(i));
      if (ensure_capacity) must_zero = false;
    }

    if (must_zero) std::memset(buffer_ + i - length_, 0, length_);
    length_ = i;
  }

  std::memcpy(buffer_ + position_, buffer + offset, count);
  position_ = i;
  return Status();
}

Status MemoryStream::Write(ReadOnlySpan<unsigned char> buffer) {
  RETURN_IF_ERROR(EnsureNotClosed());
  RETURN_IF_ERROR(EnsureWriteable());
  if (buffer.Length() > kMaxLength - position_) STREAM_THROW_TOO_LONG();
  offset_t i = position_ + buffer.Length();
  if (i > length_) {
    bool must_zero = position_ > length_;
    if (i > capacity_) {
      bool ensure_capacity;
      ASSIGN_OR_RETURN(ensure_capacity, EnsureCapacity(i));
      if (ensure_capacity) must_zero = false;
    }

    if (must_zero) std::memset(buffer_ + i - length_, 0, length_);
    length_ = i;
  }

  std::memcpy(buffer_ + position_, buffer, buffer.Length());
  position_ = i;
  return Status();
}

Status MemoryStream::WriteByte(unsigned char value) {
  RETURN_IF_ERROR(EnsureNotClosed());
  RETURN_IF_ERROR(EnsureWriteable());
  if (position_ >= length_) {
    offset_t new_length = position_ + 1;
    bool must_zero = position_ > length_;
    if (new_length >= capacity_) {
      bool ensure_capacity;
      ASSIGN_OR_RETURN(ensure_capacity, EnsureCapacity(new_length));
      if (ensure_capacity) must_zero = false;
    }

    if (must_zero) std::memset(buffer_ + position_ - length_, 0, length_);
    length_ = new_length;
  }

  buffer_[static_cast<size_t>(position_++)] = value;
  return Status();
}

StatusOr<Array<unsigned char>> MemoryStream::ToArray() {
  RETURN_IF_ERROR(EnsureNotClosed());
  if (length_ == 0) return Array<unsigned char>();
  Array<unsigned char> array(length_);
  std::memcpy(array, buffer_, length_);
  return array;
}

void MemoryStream::Close() { Dispose(); }

void MemoryStream::Dispose() {
  writable_ = false;
  is_open_ = false;
  exposable_ = false;
}

Status MemoryStream::CopyTo(Stream& stream, offset_t buffer_size) {
  RETURN_IF_ERROR(ValidateCopyToArguments(stream, buffer_size));
  RETURN_IF_ERROR(EnsureNotClosed());
  offset_t original_pos = position_;
  offset_t remaining = Skip(length_ - original_pos);
  if (remaining > 0)
    RETURN_IF_ERROR(stream.Write(buffer_, position_, remaining));

  return Status();
}

Status MemoryStream::EnsureNotClosed() const {
  if (!is_open_) STREAM_THROW_CLOSED();
  return Status();
}

Status MemoryStream::EnsureWriteable() const {
  if (!CanWrite()) STREAM_THROW_UNWRITABLE();
  return Status();
}

StatusOr<bool> MemoryStream::EnsureCapacity(offset_t value) {
  static constexpr const offset_t k256 = 256;
  if (value > capacity_) {
    offset_t default_new_capacity = capacity_ * 2;
    offset_t new_capacity = std::max(value, k256);
    if (default_new_capacity > kMaxLength)
      new_capacity = std::max(value, kMaxLength);
    else if (new_capacity < default_new_capacity)
      new_capacity = default_new_capacity;

    RETURN_IF_ERROR(SetCapacity(new_capacity));
    return true;
  }

  return false;
}

offset_t MemoryStream::Skip(offset_t count) {
  offset_t n = length_ - position_;
  if (n > count) n = count;
  if (n < 0) n = 0;
  assert(n <= kMaxLength - position_ && "n <= kMaxLength - position_");
  position_ += n;
  return n;
}

MemoryStream& MemoryStream::operator=(MemoryStream&& other) noexcept {
  if (this != &other) {
    buffer_ = std::move(other.buffer_);
    position_ = other.position_;
    length_ = other.length_;
    capacity_ = other.capacity_;
    is_open_ = other.is_open_;
    writable_ = other.writable_;
    expandable_ = other.expandable_;
    exposable_ = other.exposable_;

    other.writable_ = false;
    other.is_open_ = false;
    other.exposable_ = false;
  }

  return *this;
}

}  // namespace io
}  // namespace ws
