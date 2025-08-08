#include "ws/io/stream.h"

namespace ws {
namespace io {

Status Stream::CopyTo(Stream& stream) {
  return CopyTo(stream, kDefaultCopyBufferSize);
}

Status Stream::ValidateBufferArguments(std::span<const value_type> buffer,
                                       size_type offset, size_type count) {
  if (offset < 0 || offset > buffer.size())
    return Status(StatusCode::kOutOfRange, "Offset out of range");

  if (count > buffer.size() - offset)
    return Status(StatusCode::kOutOfRange, "Count out of range");

  return Status();
}

Status Stream::ValidateCopyToArguments(Stream& stream, size_type buffer_size) {
  if (buffer_size <= 0)
    return Status(StatusCode::kBadRequest,
                  "Buffer size must be greater than zero");

  if (!stream.CanWrite()) {
    if (stream.CanRead()) STREAM_THROW_UNWRITABLE();
    STREAM_THROW_CLOSED();
  }

  return Status();
}

Status Stream::CopyTo(Stream& stream, size_type buffer_size) {
  RETURN_IF_ERROR(ValidateCopyToArguments(stream, buffer_size));
  if (!CanRead()) {
    if (CanWrite()) STREAM_THROW_UNREADABLE();
    STREAM_THROW_CLOSED();
  }

  size_type bytesRead;
  value_type ptr[buffer_size];
  std::span<value_type> buffer(ptr, buffer_size);
  while (true) {
    ASSIGN_OR_RETURN(bytesRead, Read(buffer, 0, buffer.size()));
    if (bytesRead == 0) break;
    stream.Write(buffer, 0, bytesRead);
  }

  return Status();
}

}  // namespace io
}  // namespace ws
