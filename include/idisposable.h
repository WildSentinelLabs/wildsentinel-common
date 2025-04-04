#pragma once
#include <stdexcept>
#include <type_traits>

namespace ws {

struct disposed_object_exception : public std::exception {
  const char* what() const noexcept override {
    return "Object has already been disposed";
  }
};

class IDisposable {
 public:
  virtual void Dispose() = 0;
  virtual ~IDisposable() = default;
};

template <typename T>
concept IsDisposable = std::is_base_of_v<IDisposable, T>;

}  // namespace ws
