#include "streams/file_stream.h"

FileStream::FileStream(const char* filename, bool is_writable,
                       std::ios::openmode mode)
    : is_writable_(is_writable) {
  file_handler_.open(filename, mode);
  if (is_writable && !file_handler_.is_open()) {
    file_handler_.clear();
    mode = std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc;
    file_handler_.open(filename, mode);
  }

  if (file_handler_) {
    file_handler_.seekg(0, std::ios::end);
    file_size_ = file_handler_.tellg();
    file_handler_.seekg(0, std::ios::beg);
    position_ = 0;
  } else {
    file_size_ = 0;
  }

  is_open_ = true;
}

FileStream::~FileStream() { Dispose(); }

FileStream* FileStream::Create(const char* filename) {
  std::ios::openmode mode =
      std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc;
  return new FileStream(filename, true, mode);
}

FileStream* FileStream::Open(const char* filename, bool is_writable) {
  std::ios::openmode mode = std::ios::binary;
  mode |= is_writable ? (std::ios::in | std::ios::out) : std::ios::in;
  return new FileStream(filename, is_writable, mode);
}

bool FileStream::CanRead() const { return is_open_; }

bool FileStream::CanWrite() const { return is_writable_; }

size_t FileStream::GetLength() const { return file_size_; }

size_t FileStream::GetPosition() const { return position_; }

void FileStream::SetPosition(size_t offset) {
  if (offset <= file_size_) position_ = offset;
}

size_t FileStream::Read(unsigned char buffer[], size_t offset, size_t count) {
  if (position_ >= file_size_) return 0;
  file_handler_.seekg(position_);
  size_t bytesRead = std::min(count, file_size_ - position_);
  file_handler_.read(reinterpret_cast<char*>(&buffer[offset]), bytesRead);
  position_ += bytesRead;
  file_handler_.seekp(position_);
  return bytesRead;
}

size_t FileStream::Read(unsigned char buffer[], size_t count) {
  return Read(buffer, 0, count);
}

bool FileStream::Write(const unsigned char buffer[], size_t offset,
                       size_t count) {
  if (!is_writable_) return false;
  file_handler_.seekp(position_);
  file_handler_.write(reinterpret_cast<const char*>(&buffer[offset]), count);
  position_ += count;
  file_size_ = std::max(file_size_, position_);
  file_handler_.seekg(position_);
  return true;
}

bool FileStream::WriteTo(Stream& stream) {
  if (file_size_ == 0) return false;
  std::streampos originalPos = file_handler_.tellg();
  file_handler_.seekg(0, std::ios::beg);
  const size_t bufferSize = 8192;
  unsigned char buffer[bufferSize];
  size_t totalBytesCopied = 0;
  while (file_handler_ && totalBytesCopied < file_size_) {
    file_handler_.read(reinterpret_cast<char*>(buffer), bufferSize);
    size_t bytesRead = file_handler_.gcount();
    if (stream.Write(buffer, 0, bytesRead) != bytesRead) {
      file_handler_.seekg(originalPos);
      return false;
    }

    totalBytesCopied += bytesRead;
  }

  file_handler_.seekg(originalPos);
  return true;
}

unsigned char* FileStream::ToArray() {
  if (file_size_ == 0) return nullptr;
  unsigned char* arrayCopy = new unsigned char[file_size_];
  std::streampos originalPos = file_handler_.tellg();
  file_handler_.seekg(0, std::ios::beg);
  file_handler_.read(reinterpret_cast<char*>(arrayCopy), file_size_);
  file_handler_.seekg(originalPos);
  return arrayCopy;
}

void FileStream::Dispose() {
  if (file_handler_.is_open()) file_handler_.close();
  position_ = 0;
  file_size_ = 0;
  is_writable_ = false;
  is_open_ = false;
}
