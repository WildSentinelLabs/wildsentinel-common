#pragma once
#include <climits>
#include <cstddef>
#include <string>

#include "ws/array.h"
#include "ws/io/seek_origin.h"
#include "ws/span.h"
#include "ws/status/status_or.h"
#include "ws/types.h"

// Macros for throwing exceptions
#define STREAM_THROW_UNREADABLE()                                        \
  do {                                                                   \
    return Status(StatusCode::kConflict, "IO Error: Unreadable Stream"); \
  } while (0)

#define STREAM_THROW_UNWRITABLE()                                        \
  do {                                                                   \
    return Status(StatusCode::kConflict, "IO Error: Unwritable Stream"); \
  } while (0)

#define STREAM_THROW_CLOSED()                                        \
  do {                                                               \
    return Status(StatusCode::kConflict, "IO Error: Stream Closed"); \
  } while (0)

#define STREAM_THROW_TOO_LONG()                                               \
  do {                                                                        \
    return Status(StatusCode::kPayloadTooLarge, "IO Error: Stream Too Long"); \
  } while (0)

#define STREAM_THROW_NOT_EXPANDABLE()                 \
  do {                                                \
    return Status(StatusCode::kPayloadTooLarge,       \
                  "IO Error: Stream Not Expandable"); \
  } while (0)

namespace ws {
namespace io {
class Stream {
 public:
  virtual ~Stream() = default;

  virtual bool CanSeek() = 0;
  virtual bool CanRead() const = 0;
  virtual bool CanWrite() const = 0;
  virtual offset_t Length() = 0;
  virtual offset_t Position() = 0;
  virtual Status SetPosition(offset_t value) = 0;
  virtual StatusOr<offset_t> Read(Span<unsigned char> buffer, offset_t offset,
                                  offset_t count) = 0;
  virtual StatusOr<offset_t> Read(Span<unsigned char> buffer) = 0;
  virtual StatusOr<int16_t> ReadByte() = 0;
  virtual StatusOr<offset_t> Seek(offset_t offset, SeekOrigin origin) = 0;
  virtual Status Write(ReadOnlySpan<unsigned char> buffer, offset_t offset,
                       offset_t count) = 0;
  virtual Status Write(ReadOnlySpan<unsigned char> buffer) = 0;
  virtual Status WriteByte(unsigned char value) = 0;
  virtual StatusOr<Array<unsigned char>> ToArray() = 0;
  virtual Status CopyTo(Stream& stream);
  virtual void Close() = 0;
  virtual void Dispose() = 0;

 protected:
#if defined(_WIN32)
// En Windows, la API usa enteros de 64 bits (LARGE_INTEGER) incluso en sistemas
// de 32 bits.
#if UINTPTR_MAX == 0xffffffffffffffffULL              // 64 bits
  static constexpr offset_t kMaxLength = 1ULL << 34;  // 16 GB
#elif UINTPTR_MAX == 0xffffffffULL                    // 32 bits
  static constexpr offset_t kMaxLength = 1ULL << 31;  // 2 GB
#else
  static constexpr offset_t kMaxLength = 1ULL << 31;  // Default
#endif
#elif defined(__unix__)
#if UINTPTR_MAX == 0xffffffffffffffffULL  // 64 bits
  static constexpr offset_t kMaxLength = 1ULL << 34;  // 16 GB
#elif UINTPTR_MAX == 0xffffffffULL        // 32 bits
  static constexpr offset_t kMaxLength = 1ULL << 33;  // 8 GB
#else
  static constexpr offset_t kMaxLength = 1ULL << 31;  // Default
#endif
#else
  static constexpr offset_t kMaxLength = 1ULL << 31;  // Default
#endif

  static Status ValidateBufferArguments(ReadOnlySpan<unsigned char> buffer,
                                        offset_t offset, offset_t count);
  static Status ValidateSeekArguments(offset_t offset, SeekOrigin origin);
  static Status ValidateCopyToArguments(Stream& stream, offset_t buffer_size);
  virtual Status CopyTo(Stream& stream, offset_t buffer_size);

 private:
  static constexpr const offset_t kDefaultCopyBufferSize = 81920;
};

}  // namespace io
}  // namespace ws
