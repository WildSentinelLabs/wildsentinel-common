#pragma once
#include <climits>
#include <cstddef>
#include <span>
#include <string>

#include "ws/array.h"
#include "ws/io/seek_origin.h"
#include "ws/status/status_or.h"
#include "ws/types.h"

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
  using value_type = unsigned char;
  using size_type = offset_t;
  using container_type = Array<value_type>;

  virtual ~Stream() = default;

  virtual bool CanSeek() = 0;
  virtual bool CanRead() const = 0;
  virtual bool CanWrite() const = 0;
  virtual size_type Length() = 0;
  virtual size_type Position() = 0;
  virtual Status SetPosition(size_type value) = 0;
  virtual StatusOr<size_type> Read(std::span<value_type> buffer,
                                   size_type offset, size_type count) = 0;
  virtual StatusOr<size_type> Read(std::span<value_type> buffer) = 0;
  virtual StatusOr<int16_t> ReadByte() = 0;
  virtual StatusOr<size_type> Seek(size_type offset, SeekOrigin origin) = 0;
  virtual Status Write(std::span<const value_type> buffer, size_type offset,
                       size_type count) = 0;
  virtual Status Write(std::span<const value_type> buffer) = 0;
  virtual Status WriteByte(value_type value) = 0;
  virtual StatusOr<container_type> ToArray() = 0;
  virtual Status CopyTo(Stream& stream);
  virtual void Close() = 0;
  virtual void Dispose() = 0;

 protected:
#if defined(_WIN32)
// En Windows, la API usa enteros de 64 bits (LARGE_INTEGER) incluso en sistemas
// de 32 bits.
#if UINTPTR_MAX == 0xffffffffffffffffULL               // 64 bits
  static constexpr size_type kMaxLength = 1ULL << 34;  // 16 GB
#elif UINTPTR_MAX == 0xffffffffULL                     // 32 bits
  static constexpr size_type kMaxLength = 1ULL << 31;  // 2 GB
#else
  static constexpr size_type kMaxLength = 1ULL << 31;  // Default
#endif
#elif defined(__unix__)
#if UINTPTR_MAX == 0xffffffffffffffffULL  // 64 bits
  static constexpr size_type kMaxLength = 1ULL << 34;  // 16 GB
#elif UINTPTR_MAX == 0xffffffffULL        // 32 bits
  static constexpr size_type kMaxLength = 1ULL << 33;  // 8 GB
#else
  static constexpr size_type kMaxLength = 1ULL << 31;  // Default
#endif
#else
  static constexpr size_type kMaxLength = 1ULL << 31;  // Default
#endif

  static Status ValidateBufferArguments(std::span<const value_type> buffer,
                                        size_type offset, size_type count);
  static Status ValidateCopyToArguments(Stream& stream, size_type buffer_size);
  virtual Status CopyTo(Stream& stream, size_type buffer_size);

 private:
  static constexpr const size_type kDefaultCopyBufferSize = 81920;
};

}  // namespace io
}  // namespace ws
