#pragma once

#include "idisposable.h"
namespace ws {
namespace pooling {
template <typename T>
class IObjectPool : {
 public:
  virtual T Get() = 0;
  virtual void Return(const T& item) = 0;
  virtual void Return(T&& item) = 0;
  virtual void Clear() = 0;
};
}  // namespace pooling
}  // namespace ws
