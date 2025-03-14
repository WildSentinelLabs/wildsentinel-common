#pragma once

#include <iostream>
#include <utility>

namespace ws {
class IDisposable {
  virtual void Dispose() = 0;
};

}  // namespace ws
