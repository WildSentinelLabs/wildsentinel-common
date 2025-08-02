#include "ws/io/file_stream.h"

namespace ws {
namespace io {

StatusOr<FileStream> FileStream::Create(const std::string& path, FileMode mode,
                                        FileAccess access, FileShare share) {
  return Create(path, mode, access, share, kDefaultBufferSize);
}

StatusOr<FileStream> FileStream::Create(const std::string& path, FileMode mode,
                                        FileAccess access, FileShare share,
                                        offset_t buffer_size) {
  return Create(path, mode, access, share, buffer_size, 0);
}

StatusOr<FileStream> FileStream::Create(const std::string& path, FileMode mode,
                                        FileAccess access, FileShare share,
                                        offset_t buffer_size,
                                        offset_t preallocation_size) {
  std::filesystem::path full_path = std::filesystem::absolute(path);
  offset_t position = 0;
  offset_t append_start = -1;
  FileHandle file_handle;
  ASSIGN_OR_RETURN(file_handle, FileHandle::Open(full_path, mode, access, share,
                                                 preallocation_size));
  if (mode == FileMode::kAppend && file_handle.CanSeek()) {
    offset_t length;
    ASSIGN_OR_CLEANUP(length, file_handle.FileLength(),
                      { file_handle.Dispose(); });
    append_start = position = length;
  }

  return FileStream(file_handle, position, append_start, access);
}

FileStream::FileStream()
    : file_handle_(FileHandle()),
      position_(-1),
      append_start_(-1),
      access_(static_cast<FileAccess>(0)) {}

FileStream::FileStream(FileHandle file_handle, offset_t position,
                       offset_t append_start, FileAccess access)
    : file_handle_(file_handle),
      position_(position),
      append_start_(append_start),
      access_(access) {}

FileStream::FileStream(FileStream&& other) noexcept
    : file_handle_(std::move(other.file_handle_)),
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

offset_t FileStream::Length() {
  auto length = file_handle_.FileLength();
  if (!length.Ok()) return 0;
  return length.Value();
}

offset_t FileStream::Position() { return position_; }

Status FileStream::SetPosition(offset_t value) {
  return Seek(value, SeekOrigin::kBegin).GetStatus();
}

Status FileStream::SetLength(offset_t value) {
  if (value < 0)
    return Status(StatusCode::kBadRequest, "Negative length not allowed");

  if (value > kMaxLength) STREAM_THROW_TOO_LONG();
  if (append_start_ != -1 && value < append_start_)
    return Status(StatusCode::kRuntimeError,
                  "IO Error: SetLengthAppendTruncate");

  RETURN_IF_ERROR(FileHandle::SetFileLength(file_handle_, value));
  offset_t cached_length;
  assert(!file_handle_.TryGetCachedLength(cached_length) &&
         "If length can be cached (file opened for reading, not shared for "
         "writing), it should be impossible to modify file length");
  if (position_ > value) position_ = value;
  return Status();
}

StatusOr<offset_t> FileStream::Read(Span<unsigned char> buffer, offset_t offset,
                                    offset_t count) {
  RETURN_IF_ERROR(ValidateBufferArguments(buffer, offset, count));
  Span<unsigned char> span(buffer, offset, count);
  return Read(span);
}

StatusOr<offset_t> FileStream::Read(Span<unsigned char> buffer) {
  if (file_handle_.IsClosed())
    STREAM_THROW_CLOSED();
  else if ((access_ & FileAccess::kRead) == static_cast<FileAccess>(0))
    STREAM_THROW_UNREADABLE();

  offset_t r;
  ASSIGN_OR_RETURN(r,
                   FileHandle::ReadAtOffset(file_handle_, buffer, position_));
  assert(r >= 0 && "ReadAtOffset failed");
  position_ += r;
  return r;
}

StatusOr<int16_t> FileStream::ReadByte() {
  unsigned char b = 0;
  Span<unsigned char> span(&b, 1);
  offset_t r;
  ASSIGN_OR_RETURN(r, Read(span));
  return r != 0 ? b : -1;
}

StatusOr<offset_t> FileStream::Seek(offset_t offset, SeekOrigin origin) {
  offset_t old_pos = position_;
  offset_t pos = 0;
  switch (origin) {
    case SeekOrigin::kBegin:
      pos = offset;
      break;
    case SeekOrigin::kEnd:
      ASSIGN_OR_RETURN(pos, file_handle_.FileLength());
      pos += offset;
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
    return Status(StatusCode::kBadRequest, "Negative position not allowed");
  }

  if (append_start_ != -1 && pos < append_start_) {
    position_ = old_pos;
    return Status(StatusCode::kRuntimeError, "IO Error: SeekAppendOverwrite");
  }

  return pos;
}

Status FileStream::Write(ReadOnlySpan<unsigned char> buffer, offset_t offset,
                         offset_t count) {
  RETURN_IF_ERROR(ValidateBufferArguments(buffer, offset, count));
  ReadOnlySpan<unsigned char> span(buffer, offset, count);
  return Write(span);
}

Status FileStream::Write(ReadOnlySpan<unsigned char> buffer) {
  if (file_handle_.IsClosed())
    STREAM_THROW_CLOSED();
  else if ((access_ & FileAccess::kWrite) == static_cast<FileAccess>(0))
    STREAM_THROW_UNWRITABLE();

  RETURN_IF_ERROR(FileHandle::WriteAtOffset(file_handle_, buffer, position_));
  position_ += buffer.Length();
  return Status();
}

Status FileStream::WriteByte(unsigned char value) {
  Span<unsigned char> span(&value, 1);
  return Write(span);
}

StatusOr<Array<unsigned char>> FileStream::ToArray() {
  offset_t length = Length();
  Array<unsigned char> array(length);
  offset_t old_pos = position_;
  RETURN_IF_ERROR(SetPosition(0));
  offset_t bytes_read;
  ASSIGN_OR_RETURN(bytes_read, Read(array, 0, array.Length()));
  position_ = old_pos;
  if (bytes_read != array.Length())
    return Status(StatusCode::kRuntimeError,
                  "IO Error: Read did not read the expected number of bytes");

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
    file_handle_ = std::move(other.file_handle_);
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
