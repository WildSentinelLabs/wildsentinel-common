#pragma once
#include <cstddef>

#include "idisposable.h"
namespace ws {
namespace io {

class Stream : public IDisposable {
 public:
  virtual ~Stream() = default;

  virtual bool CanRead() const = 0;

  virtual bool CanWrite() const = 0;

  virtual size_t GetLength() const = 0;

  virtual size_t GetPosition() const = 0;

  virtual void SetPosition(size_t offset) = 0;

  virtual size_t Read(unsigned char buffer[], size_t offset, size_t count) = 0;

  virtual size_t Read(unsigned char buffer[], size_t count) = 0;

  virtual bool Write(const unsigned char buffer[], size_t offset,
                     size_t count) = 0;

  virtual bool WriteTo(Stream& stream) = 0;

  virtual unsigned char* ToArray() = 0;
};
}  // namespace io
}  // namespace ws
