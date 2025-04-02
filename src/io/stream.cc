#include "io/stream.h"

namespace ws {
namespace io {

const char* unreadable_stream_exception::what() const noexcept {
  return "Unreadable Stream";
}

const char* unwritable_stream_exception::what() const noexcept {
  return "Unwritable Stream";
}

const char* stream_closed_exception::what() const noexcept {
  return "Stream Closed";
}

const char* stream_too_long_exception::what() const noexcept {
  return "Stream Too Long";
}

void Stream::CopyTo(Stream& stream) { CopyTo(stream, kDefaultCopyBufferSize); }

void Stream::ValidateBufferArguments(ReadOnlySpan<unsigned char> buffer,
                                     offset_t offset, offset_t count) {
  if (offset < 0 || offset > buffer.Length())
    throw std::runtime_error("Offset out of range");
  if (count > buffer.Length() - offset)
    throw std::runtime_error("Count out of range");
}

void Stream::ValidateCopyToArguments(Stream& stream, offset_t buffer_size) {
  if (buffer_size <= 0) throw std::runtime_error("Size Negative or Zero");
  if (!stream.CanWrite()) {
    if (stream.CanRead()) throw unwritable_stream_exception();
    throw stream_closed_exception();
  }
}

void Stream::CopyTo(Stream& stream, offset_t buffer_size) {
  ValidateCopyToArguments(stream, buffer_size);
  if (!CanRead()) {
    if (CanWrite()) {
      throw unreadable_stream_exception();
    }

    throw stream_closed_exception();
  }

  int bytesRead;
  unsigned char ptr[buffer_size];
  Span<unsigned char> buffer(ptr, buffer_size);
  while ((bytesRead = Read(buffer, 0, buffer.Length())) != 0) {
    stream.Write(buffer, 0, bytesRead);
  }
}

}  // namespace io
}  // namespace ws
