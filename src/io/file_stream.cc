#include "io/file_stream.h"

namespace ws {
namespace io {

FileStream::FileStream()
    : file_handle_(FileHandle()),
      position_(-1),
      append_start_(0),
      access_(static_cast<FileAccess>(0)) {}

FileStream::FileStream(const std::string& path, FileMode mode)
    : FileStream(path, mode,
                 mode == FileMode::kAppend
                     ? FileAccess::kWrite
                     : FileAccess::kRead | FileAccess::kWrite,
                 kDefaultShare, kDefaultBufferSize) {}

FileStream::FileStream(const std::string& path, FileMode mode,
                       FileAccess access)
    : FileStream(path, mode, access, kDefaultShare, kDefaultBufferSize) {}

FileStream::FileStream(const std::string& path, FileMode mode,
                       FileAccess access, FileShare share)
    : FileStream(path, mode, access, share, kDefaultBufferSize) {}

FileStream::FileStream(const std::string& path, FileMode mode,
                       FileAccess access, FileShare share, offset_t buffer_size)
    : FileStream(path, mode, access, share, buffer_size, 0) {}

FileStream::FileStream(const std::string& path, FileMode mode,
                       FileAccess access, FileShare share, offset_t buffer_size,
                       offset_t preallocation_size)
    : file_handle_(FileHandle()),
      position_(0),
      append_start_(-1),
      access_(static_cast<FileAccess>(0)) {
  std::filesystem::path full_path = std::filesystem::absolute(path);
  access_ = access;
  file_handle_ =
      FileHandle::Open(full_path, mode, access, share, preallocation_size);
  try {
    if (mode == FileMode::kAppend && CanSeek()) {
      append_start_ = position_ = Length();
    } else {
      append_start_ = -1;
    }
  } catch (const std::exception& e) {
    file_handle_.Dispose();
    throw std::runtime_error("Failed to open file: " + std::string(e.what()));
  }
}

FileStream::FileStream(FileStream&& other) noexcept
    : file_handle_(std::move(file_handle_)),
      position_(other.position_),
      append_start_(other.append_start_),
      access_(other.access_) {
  other.file_handle_ = FileHandle();
  other.append_start_ = -1;
}

FileStream::~FileStream() { Dispose(); }

std::string FileStream::Name() const { return file_handle_.Path().string(); }

bool FileStream::CanSeek() { return file_handle_.CanSeek(); }

bool FileStream::CanRead() const {
  return !file_handle_.IsClosed() &&
         (access_ & FileAccess::kRead) != static_cast<FileAccess>(0);
}

bool FileStream::CanWrite() const {
  return !file_handle_.IsClosed() &&
         (access_ & FileAccess::kWrite) != static_cast<FileAccess>(0);
}

offset_t FileStream::Length() { return file_handle_.FileLength(); }

offset_t FileStream::Position() { return position_; }

void FileStream::SetPosition(offset_t value) {
  Seek(value, SeekOrigin::kBegin);
}

void FileStream::SetLength(offset_t value) {
  if (value < 0) throw std::underflow_error("Negative length not allowed");
  if (value > kMaxLength) throw stream_too_long_exception();
  if (append_start_ != -1 && value < append_start_)
    throw std::runtime_error("IO SetLengthAppendTruncate");

  FileHandle::SetFileLength(file_handle_, value);
  offset_t cached_length;
  assert(!file_handle_.TryGetCachedLength(cached_length) &&
         "If length can be cached (file opened for reading, not shared for "
         "writing), it should be impossible to modify file length");
  if (position_ > value) position_ = value;
}

offset_t FileStream::Read(Span<unsigned char> buffer, offset_t offset,
                          offset_t count) {
  ValidateBufferArguments(buffer, offset, count);
  Span<unsigned char> span(buffer, offset, count);
  return Read(span);
}

offset_t FileStream::Read(Span<unsigned char> buffer) {
  if (file_handle_.IsClosed())
    throw disposed_object_exception();
  else if ((access_ & FileAccess::kRead) == static_cast<FileAccess>(0))
    throw unreadable_stream_exception();

  offset_t r = FileHandle::ReadAtOffset(file_handle_, buffer, position_);
  assert(r >= 0 && "ReadAtOffset failed");
  position_ += r;
  return r;
}

int16_t FileStream::ReadByte() {
  unsigned char b = 0;
  Span<unsigned char> span(&b, 1);
  return Read(span) != 0 ? b : -1;
}

offset_t FileStream::Seek(offset_t offset, SeekOrigin origin) {
  offset_t old_pos = position_;
  offset_t pos = 0;
  switch (origin) {
    case SeekOrigin::kBegin:
      pos = offset;
      break;
    case SeekOrigin::kEnd:
      pos = Length() + offset;
      break;
    case SeekOrigin::kCurrent:
      pos = position_ + offset;
      break;

    default:
      break;
  }

  if (pos >= 0) {
    position_ = pos;
  } else {
    throw std::invalid_argument("Offset => Negative position not allowed");
  }

  if (append_start_ != -1 && pos < append_start_) {
    position_ = old_pos;
    throw std::runtime_error("IO SeekAppendOverwrite");
  }

  return pos;
}

void FileStream::Write(ReadOnlySpan<unsigned char> buffer, offset_t offset,
                       offset_t count) {
  ValidateBufferArguments(buffer, offset, count);
  ReadOnlySpan<unsigned char> span(buffer, offset, count);
  Write(span);
}

void FileStream::Write(ReadOnlySpan<unsigned char> buffer) {
  if (file_handle_.IsClosed())
    throw disposed_object_exception();
  else if ((access_ & FileAccess::kWrite) == static_cast<FileAccess>(0))
    throw unwritable_stream_exception();

  FileHandle::WriteAtOffset(file_handle_, buffer, position_);
  position_ += buffer.Length();
}

void FileStream::WriteByte(unsigned char value) {
  Span<unsigned char> span(&value, 1);
  Write(span);
}

Array<unsigned char> FileStream::ToArray() {
  Array<unsigned char> array(Length());
  offset_t old_pos = position_;
  Write(array, 0, array.Length());
  position_ = old_pos;
  return array;
}

void FileStream::Close() { Dispose(); }

void FileStream::Dispose() {
  if (!file_handle_.IsClosed()) {
    file_handle_.Dispose();
  }
}

FileStream& FileStream::operator=(FileStream&& other) noexcept {
  if (this != &other) {
    file_handle_ = std::move(file_handle_);
    position_ = other.position_;
    append_start_ = other.append_start_;
    access_ = other.access_;

    other.file_handle_ = FileHandle();
    other.append_start_ = -1;
  }

  return *this;
}

}  // namespace io
}  // namespace ws
