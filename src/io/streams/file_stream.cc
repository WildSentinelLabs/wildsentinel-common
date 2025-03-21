#include "io/streams/file_stream.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace ws {
namespace io {

#ifdef _WIN32

FileStream::FileStream(const char* filename, bool is_writable,
                       unsigned long desiredAccess,
                       unsigned long creationDisposition)
    : file_size_(0),
      position_(0),
      is_open_(false),
      is_writable_(is_writable),
      disposed_(false) {
  file_handle_ =
      CreateFileA(filename, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE,
                  nullptr, creationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);

  if (file_handle_ == INVALID_HANDLE_VALUE) {
    throw std::runtime_error("Failed to open file");
  }

  LARGE_INTEGER size;
  if (!GetFileSizeEx((HANDLE)file_handle_, &size)) {
    CloseHandle((HANDLE)file_handle_);
    throw std::runtime_error("Failed to get file size");
  }
  file_size_ = static_cast<size_t>(size.QuadPart);
  is_open_ = true;
}

FileStream* FileStream::Create(const char* filename) {
  return new FileStream(filename, true, GENERIC_READ | GENERIC_WRITE,
                        CREATE_ALWAYS);
}

FileStream* FileStream::Open(const char* filename, bool is_writable) {
  DWORD access = is_writable ? (GENERIC_READ | GENERIC_WRITE) : GENERIC_READ;
  return new FileStream(filename, is_writable, access, OPEN_EXISTING);
}

FileStream::~FileStream() { Dispose(); }

bool FileStream::CanRead() const { return is_open_ && !disposed_.load(); }

bool FileStream::CanWrite() const { return is_writable_ && !disposed_.load(); }

size_t FileStream::GetLength() const { return file_size_; }

size_t FileStream::GetPosition() const { return position_; }

void FileStream::SetPosition(size_t offset) {
  if (offset <= file_size_) {
    LARGE_INTEGER li;
    li.QuadPart = offset;
    if (SetFilePointerEx((HANDLE)file_handle_, li, nullptr, FILE_BEGIN)) {
      position_ = offset;
    }
  }
}

size_t FileStream::Read(unsigned char buffer[], size_t offset, size_t count) {
  if (disposed_.load()) throw disposed_object_exception();
  if (position_ >= file_size_) return 0;
  DWORD bytesToRead =
      static_cast<DWORD>(std::min(count, file_size_ - position_));
  DWORD bytesRead = 0;
  BOOL success = ReadFile((HANDLE)file_handle_, buffer + offset, bytesToRead,
                          &bytesRead, nullptr);
  if (!success) throw std::runtime_error("ReadFile failed");
  position_ += bytesRead;
  return bytesRead;
}

size_t FileStream::Read(unsigned char buffer[], size_t count) {
  return Read(buffer, 0, count);
}

bool FileStream::Write(const unsigned char buffer[], size_t offset,
                       size_t count) {
  if (disposed_.load()) throw disposed_object_exception();
  if (!is_writable_) return false;
  DWORD bytesWritten = 0;
  BOOL success = WriteFile((HANDLE)file_handle_, buffer + offset,
                           static_cast<DWORD>(count), &bytesWritten, nullptr);
  if (!success || bytesWritten != count) return false;
  position_ += bytesWritten;
  file_size_ = std::max(file_size_, position_);
  return true;
}

bool FileStream::WriteTo(Stream& stream) {
  if (disposed_.load()) throw disposed_object_exception();
  if (file_size_ == 0) return false;
  LARGE_INTEGER origPos;
  if (!SetFilePointerEx((HANDLE)file_handle_, {0}, &origPos, FILE_CURRENT)) {
    return false;
  }
  size_t original = position_;
  SetPosition(0);
  constexpr size_t bufferSize = 8192;
  unsigned char buffer[bufferSize];
  size_t totalCopied = 0;
  while (totalCopied < file_size_) {
    DWORD bytesRead = 0;
    BOOL success =
        ReadFile((HANDLE)file_handle_, buffer, bufferSize, &bytesRead, nullptr);
    if (!success) {
      SetPosition(original);
      return false;
    }
    if (stream.Write(buffer, 0, bytesRead) != bytesRead) {
      SetPosition(original);
      return false;
    }
    totalCopied += bytesRead;
  }
  SetPosition(original);
  return true;
}

unsigned char* FileStream::ToArray() {
  if (disposed_.load()) throw disposed_object_exception();
  if (file_size_ == 0) return nullptr;
  unsigned char* arrayCopy = new unsigned char[file_size_];
  LARGE_INTEGER origPos;
  if (!SetFilePointerEx((HANDLE)file_handle_, {0}, &origPos, FILE_CURRENT)) {
    delete[] arrayCopy;
    throw std::runtime_error("Failed to get current position");
  }
  size_t original = position_;
  SetPosition(0);
  DWORD bytesRead = 0;
  BOOL success = ReadFile((HANDLE)file_handle_, arrayCopy,
                          static_cast<DWORD>(file_size_), &bytesRead, nullptr);
  if (!success || bytesRead != file_size_) {
    delete[] arrayCopy;
    SetPosition(original);
    throw std::runtime_error("Failed to read entire file");
  }
  SetPosition(original);
  return arrayCopy;
}

void FileStream::Dispose() {
  if (disposed_.exchange(true)) return;
  if (file_handle_ && file_handle_ != INVALID_HANDLE_VALUE) {
    CloseHandle((HANDLE)file_handle_);
    file_handle_ = INVALID_HANDLE_VALUE;
  }
  position_ = 0;
  file_size_ = 0;
  is_writable_ = false;
  is_open_ = false;
}

#else
// TODO: USE DISPOSED_ IN LINUX
FileStream::FileStream(const char* filename, bool is_writable, int flags,
                       int mode)
    : file_size_(0),
      position_(0),
      is_open_(false),
      is_writable_(is_writable),
      disposed_(false) {
  fd_ = open(filename, flags, mode);
  if (fd_ < 0) {
    throw std::runtime_error("Failed to open file");
  }
  struct stat st;
  if (fstat(fd_, &st) == 0) {
    file_size_ = static_cast<size_t>(st.st_size);
  }
  is_open_ = true;
}

FileStream* FileStream::Create(const char* filename) {
  int flags = O_RDWR | O_CREAT | O_TRUNC;
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
  return new FileStream(filename, true, flags, mode);
}

FileStream* FileStream::Open(const char* filename, bool is_writable) {
  int flags = is_writable ? O_RDWR : O_RDONLY;
  return new FileStream(filename, is_writable, flags, 0);
}

FileStream::~FileStream() { Dispose(); }

bool FileStream::CanRead() const { return is_open_ && !disposed_.load(); }

bool FileStream::CanWrite() const { return is_writable_ && !disposed_.load(); }

size_t FileStream::GetLength() const { return file_size_; }

size_t FileStream::GetPosition() const { return position_; }

void FileStream::SetPosition(size_t offset) {
  if (offset <= file_size_) {
    off_t res = lseek(fd_, offset, SEEK_SET);
    if (res != (off_t)-1) {
      position_ = offset;
    }
  }
}

size_t FileStream::Read(unsigned char buffer[], size_t offset, size_t count) {
  if (disposed_.load()) throw disposed_object_exception();
  if (position_ >= file_size_) return 0;
  size_t bytesToRead = std::min(count, file_size_ - position_);
  ssize_t bytesRead = read(fd_, buffer + offset, bytesToRead);
  if (bytesRead < 0) throw std::runtime_error("read() failed");
  position_ += bytesRead;
  return bytesRead;
}

size_t FileStream::Read(unsigned char buffer[], size_t count) {
  return Read(buffer, 0, count);
}

bool FileStream::Write(const unsigned char buffer[], size_t offset,
                       size_t count) {
  if (disposed_.load()) throw disposed_object_exception();
  if (!is_writable_) return false;
  ssize_t bytesWritten = write(fd_, buffer + offset, count);
  if (bytesWritten < 0 || static_cast<size_t>(bytesWritten) != count)
    return false;
  position_ += bytesWritten;
  file_size_ = std::max(file_size_, position_);
  return true;
}

bool FileStream::WriteTo(Stream& stream) {
  if (disposed_.load()) throw disposed_object_exception();
  if (file_size_ == 0) return false;
  off_t originalPos = lseek(fd_, 0, SEEK_CUR);
  SetPosition(0);
  constexpr size_t bufferSize = 8192;
  unsigned char buffer[bufferSize];
  size_t totalCopied = 0;
  while (totalCopied < file_size_) {
    ssize_t bytesRead = read(fd_, buffer, bufferSize);
    if (bytesRead < 0) {
      lseek(fd_, originalPos, SEEK_SET);
      return false;
    }
    if (stream.Write(buffer, 0, bytesRead) != static_cast<size_t>(bytesRead)) {
      lseek(fd_, originalPos, SEEK_SET);
      return false;
    }
    totalCopied += bytesRead;
  }
  lseek(fd_, originalPos, SEEK_SET);
  return true;
}

unsigned char* FileStream::ToArray() {
  if (disposed_.load()) throw disposed_object_exception();
  if (file_size_ == 0) return nullptr;
  unsigned char* arrayCopy = new unsigned char[file_size_];
  off_t originalPos = lseek(fd_, 0, SEEK_CUR);
  SetPosition(0);
  ssize_t bytesRead = read(fd_, arrayCopy, file_size_);
  if (bytesRead < 0 || static_cast<size_t>(bytesRead) != file_size_) {
    delete[] arrayCopy;
    lseek(fd_, originalPos, SEEK_SET);
    throw std::runtime_error("Failed to read entire file");
  }
  lseek(fd_, originalPos, SEEK_SET);
  return arrayCopy;
}

void FileStream::Dispose() {
  if (disposed_.exchange(true)) return;
  if (fd_ >= 0) {
    close(fd_);
    fd_ = -1;
  }

  position_ = 0;
  file_size_ = 0;
  is_writable_ = false;
  is_open_ = false;
}

#endif

}  // namespace io
}  // namespace ws
