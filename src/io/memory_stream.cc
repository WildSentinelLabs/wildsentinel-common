#include "io/memory_stream.h"
namespace ws {
namespace io {

MemoryStream::MemoryStream() : MemoryStream(0) {}

MemoryStream::MemoryStream(offset_t capacity)
    : buffer_(capacity > 0 ? std::move(Array<unsigned char>(capacity))
                           : Array<unsigned char>::Empty()),
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
  other = MemoryStream();
}

MemoryStream::~MemoryStream() { Dispose(); }

bool MemoryStream::CanSeek() { return is_open_; }

bool MemoryStream::CanRead() const { return is_open_; }

bool MemoryStream::CanWrite() const { return writable_; }

offset_t MemoryStream::Length() {
  EnsureNotClosed();
  return length_;
}

offset_t MemoryStream::Position() {
  EnsureNotClosed();
  return position_;
}

offset_t MemoryStream::Capacity() const {
  EnsureNotClosed();
  return capacity_;
};

bool MemoryStream::TryGetBuffer(Span<unsigned char>& buffer) const {
  if (!exposable_) {
    buffer = Span<unsigned char>::Empty();
    return false;
  }

  buffer = buffer_;
  return true;
}

void MemoryStream::SetPosition(offset_t value) {
  if (value < 0) throw std::underflow_error("Negative position not allowed");
  EnsureNotClosed();
  if (value > kMaxLength) throw stream_too_long_exception();
  position_ = value;
}

void MemoryStream::SetLength(offset_t value) {
  if (value < 0) throw std::underflow_error("Negative length not allowed");
  if (value > kMaxLength) throw stream_too_long_exception();
  EnsureWriteable();
  if (!EnsureCapacity(value) && value > length_)
    std::memset(buffer_ + value - length_, 0, length_);
  length_ = value;
  if (position_ > value) position_ = value;
}

void MemoryStream::SetCapacity(offset_t value) {
  if (value < length_) throw std::runtime_error("Small capacity out of range");
  EnsureNotClosed();
  if (!expandable_ && (value != Capacity()))
    throw std::runtime_error("Stream not expandable");
  else if (expandable_ && value != capacity_) {
    if (value > 0) {
      Array<unsigned char> new_buffer(value);
      if (length_ > 0) std::memcpy(new_buffer, buffer_, length_);
      buffer_ = std::move(new_buffer);
    } else {
      buffer_ = std::move(Array<unsigned char>::Empty());
    }

    capacity_ = value;
  }
}

offset_t MemoryStream::Read(Span<unsigned char> buffer, offset_t offset,
                            offset_t count) {
  ValidateBufferArguments(buffer, offset, count);
  EnsureNotClosed();
  offset_t n = length_ - position_;
  if (n > count) n = count;
  if (n <= 0) return 0;
  assert(n <= kMaxLength - position_ && "n <= kMaxLength - position_");
  std::memcpy(buffer + offset, buffer_ + position_, n);
  position_ += n;
  return n;
}

offset_t MemoryStream::Read(Span<unsigned char> buffer) {
  EnsureNotClosed();
  offset_t n =
      std::min(static_cast<offset_t>(buffer.Length()), length_ - position_);
  if (n <= 0) return 0;
  std::memcpy(buffer, buffer_ + position_, n);
  position_ += n;
  return n;
}

int16_t MemoryStream::ReadByte() {
  EnsureNotClosed();
  if (position_ >= length_) return -1;
  return static_cast<int16_t>(buffer_[static_cast<size_t>(position_++)]);
}

offset_t MemoryStream::Seek(offset_t offset, SeekOrigin origin) {
  EnsureNotClosed();
  switch (origin) {
    case SeekOrigin::kBegin:
      if (offset < 0 || offset > kMaxLength)
        throw std::runtime_error("IO SeekBeforeBegin");
      position_ = offset;
      break;
    case SeekOrigin::kEnd:
      if (offset > kMaxLength - length_ || offset > -length_)
        throw std::runtime_error("IO SeekBeforeBegin");
      position_ = length_ + offset;
      break;
    case SeekOrigin::kCurrent:
    default:
      if (offset > kMaxLength - position_ || offset > -position_)
        throw std::runtime_error("IO SeekBeforeBegin");
      position_ = position_ + offset;
      break;
  }

  assert(position_ <= kMaxLength && "position_ <= kMaxLength");
  return position_;
}

void MemoryStream::Write(ReadOnlySpan<unsigned char> buffer, offset_t offset,
                         offset_t count) {
  ValidateBufferArguments(buffer, offset, count);
  EnsureNotClosed();
  EnsureWriteable();
  if (count > kMaxLength - position_) throw stream_too_long_exception();
  offset_t i = position_ + count;
  if (i > length_) {
    bool must_zero = position_ > length_;
    if (i > capacity_) {
      bool allocated = EnsureCapacity(i);
      if (allocated) must_zero = false;
    }

    if (must_zero) std::memset(buffer_ + i - length_, 0, length_);
    length_ = i;
  }

  std::memcpy(buffer_ + position_, buffer + offset, count);
  position_ = i;
}

void MemoryStream::Write(ReadOnlySpan<unsigned char> buffer) {
  EnsureNotClosed();
  EnsureWriteable();
  if (buffer.Length() > kMaxLength - position_)
    throw stream_too_long_exception();
  offset_t i = position_ + buffer.Length();
  if (i > length_) {
    bool must_zero = position_ > length_;
    if (i > capacity_) {
      bool allocated = EnsureCapacity(i);
      if (allocated) must_zero = false;
    }

    if (must_zero) std::memset(buffer_ + i - length_, 0, length_);
    length_ = i;
  }

  std::memcpy(buffer_ + position_, buffer, buffer.Length());
  position_ = i;
}

void MemoryStream::WriteByte(unsigned char value) {
  EnsureNotClosed();
  EnsureWriteable();
  if (position_ >= length_) {
    offset_t new_length = position_ + 1;
    bool must_zero = position_ > length_;
    if (new_length >= capacity_) {
      bool allocated = EnsureCapacity(new_length);
      if (allocated) must_zero = false;
    }

    if (must_zero) std::memset(buffer_ + position_ - length_, 0, length_);
    length_ = new_length;
  }

  buffer_[static_cast<size_t>(position_++)] = value;
}

Array<unsigned char> MemoryStream::ToArray() {
  EnsureNotClosed();
  if (length_ == 0) return Array<unsigned char>::Empty();
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

void MemoryStream::CopyTo(Stream& stream, offset_t buffer_size) {
  ValidateCopyToArguments(stream, buffer_size);
  EnsureNotClosed();
  offset_t original_pos = position_;
  offset_t remaining = Skip(length_ - original_pos);
  if (remaining > 0) stream.Write(buffer_, position_, remaining);
}

void MemoryStream::EnsureNotClosed() const {
  if (!is_open_) throw stream_closed_exception();
}

void MemoryStream::EnsureWriteable() const {
  if (!CanWrite()) throw unwritable_stream_exception();
}

bool MemoryStream::EnsureCapacity(offset_t value) {
  static constexpr const offset_t k256 = 256;
  if (value > capacity_) {
    offset_t default_new_capacity = capacity_ * 2;
    offset_t new_capacity = std::max(value, k256);
    if (default_new_capacity > kMaxLength)
      new_capacity = std::max(value, kMaxLength);
    else if (new_capacity < default_new_capacity)
      new_capacity = default_new_capacity;
    SetCapacity(new_capacity);
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

    other = MemoryStream();
  }

  return *this;
}

}  // namespace io
}  // namespace ws
