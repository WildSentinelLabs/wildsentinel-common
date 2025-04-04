#pragma once
#include <cstdint>

namespace ws {

#if defined(_WIN32)
using offset_t = std::int64_t;
#elif defined(__unix__)
#include <sys/types.h>
using offset_t = off_t;
#else
using offset_t = std::int64_t;
#endif

}  // namespace ws
