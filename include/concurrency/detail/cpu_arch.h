#pragma once

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <cstdint>
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || \
    defined(_M_IX86)
#include <emmintrin.h>
#elif defined(__ARM_ARCH_7A__) || defined(__aarch64__) || defined(_M_ARM) || \
    defined(_M_ARM64)

#endif
#include <cassert>
#include <thread>

namespace ws {
namespace concurrency {
namespace detail {

inline constexpr size_t kMaxNfsSize = 128;

inline constexpr std::size_t kMaxNfsSizeExp = 7;

static_assert(1 << kMaxNfsSizeExp == kMaxNfsSize,
              "kMaxNfsSizeExp must be a log2(kMaxNfsSize)");

inline const std::size_t CpuLineSize() {
#if defined(_WIN32)

  DWORD buffer_size = 0;
  SYSTEM_LOGICAL_PROCESSOR_INFORMATION info;
  GetLogicalProcessorInformation(&info, &buffer_size);
  return info.Cache.LineSize ? info.Cache.LineSize : kMaxNfsSize;
#else

  long size = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
  return (size > 0) ? static_cast<std::size_t>(size) : kMaxNfsSize;
#endif
}

inline const std::size_t kNfsSize = CpuLineSize();

inline constexpr std::size_t CacheLineSize() { return kMaxNfsSize; }

inline void CpuWait(int32_t delay) {
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || \
    defined(_M_IX86)
  while (delay-- > 0) {
    _mm_pause();
  }
#elif defined(__ARM_ARCH_7A__) || defined(__aarch64__) || defined(_M_ARM) || \
    defined(_M_ARM64)
  while (delay-- > 0) {
    __asm__ __volatile__("isb sy" ::: "memory");
  }
#else /* Generic */
  (void)delay;
  std::this_thread::yield();
#endif
}

template <typename IntegerType>
constexpr bool IsPowerOfTwo(IntegerType arg) {
  static_assert(std::is_integral<IntegerType>::value,
                "An argument for IsPowerOfTwo should be integral type");
  return arg && (0 == (arg & (arg - 1)));
}

template <typename ArgIntegerType, typename DivisorIntegerType>
inline ArgIntegerType ModulusPowerOfTwo(ArgIntegerType arg,
                                        DivisorIntegerType divisor) {
  assert(IsPowerOfTwo(divisor) && "Divisor should be a power of two");
  return arg & (divisor - 1);
}

template <typename T>
constexpr bool IsAligned(T* pointer, std::uintptr_t alignment) {
  return 0 == (reinterpret_cast<std::uintptr_t>(pointer) & (alignment - 1));
}

inline void* AlignedAllocate(std::size_t bytes, std::size_t alignment) {
#if defined(_WIN32)
  return _aligned_malloc(bytes, alignment);
#else
  void* ptr = nullptr;
  if (posix_memalign(&ptr, alignment, bytes) != 0) return nullptr;
  return ptr;
#endif
}

inline void AlignedDeallocate(void* p) {
#if defined(_WIN32)
  _aligned_free(p);
#else
  free(p);
#endif
}
}  // namespace detail
}  // namespace concurrency
}  // namespace ws
