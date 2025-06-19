#pragma once
#include <exception>
#include <stdexcept>
#include <string>

namespace ws {

struct WsException : public std::exception {
 public:
  explicit WsException(const std::string& msg) : message_(msg) {}

  const char* what() const noexcept override { return message_.c_str(); }

  [[noreturn]] void Throw() const { throw *this; }

  static WsException OutOfRange(const std::string& details = "") {
    return WsException("Out of range error" + AppendDetails(details));
  }

  static WsException BadAlloc(const std::string& details = "") {
    return WsException("Memory allocation error" + AppendDetails(details));
  }

  static WsException RuntimeError(const std::string& details = "") {
    return WsException("Runtime error" + AppendDetails(details));
  }

  static WsException InvalidArgument(const std::string& details = "") {
    return WsException("Invalid argument" + AppendDetails(details));
  }

  static WsException DisposedObject(const std::string& details = "") {
    return WsException("Disposed object" + AppendDetails(details));
  }

  static WsException BadStatusOrAccess(const std::string& details = "") {
    return WsException("Bad StatusOr access" + AppendDetails(details));
  }

  static WsException OperationCanceled(const std::string& details = "") {
    return WsException("Operation canceled" + AppendDetails(details));
  }

  static WsException UnderflowError(const std::string& details = "") {
    return WsException("Underflow error" + AppendDetails(details));
  }

  static WsException IOError(const std::string& details = "") {
    return WsException("IO error" + AppendDetails(details));
  }

  static WsException InvalidFunctionCall(const std::string& details = "") {
    return WsException("Invalid function call" + AppendDetails(details));
  }

 private:
  std::string message_;

  static std::string AppendDetails(const std::string& details) {
    return details.empty() ? "" : (": " + details);
  }
};

}  // namespace ws
