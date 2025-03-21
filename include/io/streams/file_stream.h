#pragma once

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstring>
#include <stdexcept>

#include "io/stream.h"

namespace ws {
namespace io {

class FileStream : public Stream {
 public:
  ~FileStream() override;

  static FileStream* Create(const char* filename);

  static FileStream* Open(const char* filename, bool is_writable);

  bool CanRead() const override;
  bool CanWrite() const override;
  size_t GetLength() const override;
  size_t GetPosition() const override;
  void SetPosition(size_t offset) override;
  size_t Read(unsigned char buffer[], size_t offset, size_t count) override;
  size_t Read(unsigned char buffer[], size_t count) override;
  bool Write(const unsigned char buffer[], size_t offset,
             size_t count) override;
  bool WriteTo(Stream& stream) override;
  unsigned char* ToArray() override;
  void Dispose() override;

 private:
  FileStream();
#ifdef _WIN32
  FileStream(const char* filename, bool is_writable,
             unsigned long desiredAccess, unsigned long creationDisposition);
  void* file_handle_;
#else
  FileStream(const char* filename, bool is_writable, int flags, int mode);
  int fd_;
#endif
  size_t file_size_;
  size_t position_;
  bool is_open_;
  bool is_writable_;
  std::atomic<bool> disposed_;
};

}  // namespace io
}  // namespace ws
