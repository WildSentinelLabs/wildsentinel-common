#pragma once
#include <climits>
#include <cstddef>
#include <string>

#include "array.h"
#include "idisposable.h"
#include "io/seek_origin.h"
#include "span.h"
#include "types.h"
#include "wsexception.h"
namespace ws {
namespace io {
class Stream : public IDisposable {
 public:
  virtual ~Stream() = default;

  virtual bool CanSeek() = 0;
  virtual bool CanRead() const = 0;
  virtual bool CanWrite() const = 0;
  virtual offset_t Length() = 0;
  virtual offset_t Position() = 0;
  virtual void SetPosition(offset_t value) = 0;
  virtual offset_t Read(Span<unsigned char> buffer, offset_t offset,
                        offset_t count) = 0;
  virtual offset_t Read(Span<unsigned char> buffer) = 0;
  virtual int16_t ReadByte() = 0;
  virtual offset_t Seek(offset_t offset, SeekOrigin origin) = 0;
  virtual void Write(ReadOnlySpan<unsigned char> buffer, offset_t offset,
                     offset_t count) = 0;
  virtual void Write(ReadOnlySpan<unsigned char> buffer) = 0;
  virtual void WriteByte(unsigned char value) = 0;
  virtual Array<unsigned char> ToArray() = 0;
  virtual void Close() = 0;
  virtual void CopyTo(Stream& stream);

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

  static void ValidateBufferArguments(ReadOnlySpan<unsigned char> buffer,
                                      offset_t offset, offset_t count);
  static void ValidateSeekArguments(offset_t offset, SeekOrigin origin);
  static void ValidateCopyToArguments(Stream& stream, offset_t buffer_size);
  static void ThrowUnreadableStream();
  static void ThrowUnwritableStream();
  static void ThrowStreamClosed();
  static void ThrowStreamTooLong();

  virtual void CopyTo(Stream& stream, offset_t buffer_size);

 private:
  static constexpr const offset_t kDefaultCopyBufferSize = 81920;
};

}  // namespace io
}  // namespace ws
