#pragma once

#include <cstddef>
#include <fstream>
#include <iostream>

#include "io/stream.h"
namespace ws {
namespace io {
namespace streams {

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
  std::fstream file_handler_;
  size_t file_size_;
  size_t position_;
  bool is_open_;
  bool is_writable_;

  FileStream(const char* filename, bool is_writable, std::ios::openmode mode);
};
}  // namespace streams
}  // namespace io
}  // namespace ws
