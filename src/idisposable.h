#pragma once
#include <type_traits>

#include "wsexception.h"

namespace ws {
class IDisposable {
 public:
  virtual void Dispose() = 0;
  virtual ~IDisposable() = default;
};

template <typename T>
concept IsDisposable = std::is_base_of_v<IDisposable, T>;

}  // namespace ws
