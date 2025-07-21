#include "ws/io/stream.h"

namespace ws {
namespace io {

void Stream::CopyTo(Stream& stream) { CopyTo(stream, kDefaultCopyBufferSize); }

void Stream::ValidateBufferArguments(ReadOnlySpan<unsigned char> buffer,
                                     offset_t offset, offset_t count) {
  if (offset < 0 || offset > buffer.Length())
    WsException::OutOfRange("Offset out of range").Throw();
  if (count > buffer.Length() - offset)
    WsException::OutOfRange("Count out of range").Throw();
}

void Stream::ValidateCopyToArguments(Stream& stream, offset_t buffer_size) {
  if (buffer_size <= 0)
    WsException::UnderflowError("Size Negative or Zero").Throw();
  if (!stream.CanWrite()) {
    if (stream.CanRead()) ThrowUnwritableStream();
    ThrowStreamClosed();
  }
}

void Stream::CopyTo(Stream& stream, offset_t buffer_size) {
  ValidateCopyToArguments(stream, buffer_size);
  if (!CanRead()) {
    if (CanWrite()) {
      ThrowUnreadableStream();
    }

    ThrowStreamClosed();
  }

  int bytesRead;
  unsigned char ptr[buffer_size];
  Span<unsigned char> buffer(ptr, buffer_size);
  while ((bytesRead = Read(buffer, 0, buffer.Length())) != 0) {
    stream.Write(buffer, 0, bytesRead);
  }
}

void Stream::ThrowUnreadableStream() {
  WsException::IOError("Unreadable Stream").Throw();
}

void Stream::ThrowUnwritableStream() {
  WsException::IOError("Unwritable Stream").Throw();
}

void Stream::ThrowStreamClosed() {
  WsException::IOError("Stream Closed").Throw();
}

void Stream::ThrowStreamTooLong() {
  WsException::IOError("Stream Too Long").Throw();
}

}  // namespace io
}  // namespace ws
