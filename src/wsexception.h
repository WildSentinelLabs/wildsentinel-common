#pragma once

#include <exception>
#include <stdexcept>
#include <string>

namespace ws {
struct WsException : public std::exception {
 public:
  static WsException RuntimeError(const std::string& details = "");
  static WsException BadAlloc(const std::string& details = "");
  static WsException OutOfRange(const std::string& details = "");
  static WsException InvalidArgument(const std::string& details = "");
  static WsException DisposedObject(const std::string& details = "");
  static WsException BadStatusOrAccess(const std::string& details = "");
  static WsException OperationCanceled(const std::string& details = "");
  static WsException UnderflowError(const std::string& details = "");
  static WsException IOError(const std::string& details = "");
  static WsException InvalidFunctionCall(const std::string& details = "");

  explicit WsException(const std::string& msg);

  const char* what() const noexcept override;
  [[noreturn]] void Throw() const;

 private:
  std::string message_;

  static std::string AppendDetails(const std::string& details);
};

// ============================================================================
// Implementation details for WsException
// ============================================================================

inline WsException WsException::OutOfRange(const std::string& details) {
  return WsException("Out of range error" + AppendDetails(details));
}

inline WsException WsException::BadAlloc(const std::string& details) {
  return WsException("Memory allocation error" + AppendDetails(details));
}

inline WsException WsException::RuntimeError(const std::string& details) {
  return WsException("Runtime error" + AppendDetails(details));
}

inline WsException WsException::InvalidArgument(const std::string& details) {
  return WsException("Invalid argument" + AppendDetails(details));
}

inline WsException WsException::DisposedObject(const std::string& details) {
  return WsException("Disposed object" + AppendDetails(details));
}

inline WsException WsException::BadStatusOrAccess(const std::string& details) {
  return WsException("Bad StatusOr access" + AppendDetails(details));
}

inline WsException WsException::OperationCanceled(const std::string& details) {
  return WsException("Operation canceled" + AppendDetails(details));
}

inline WsException WsException::UnderflowError(const std::string& details) {
  return WsException("Underflow error" + AppendDetails(details));
}

inline WsException WsException::IOError(const std::string& details) {
  return WsException("IO error" + AppendDetails(details));
}

inline WsException WsException::InvalidFunctionCall(
    const std::string& details) {
  return WsException("Invalid function call" + AppendDetails(details));
}

inline WsException::WsException(const std::string& msg) : message_(msg) {}

inline const char* WsException::what() const noexcept {
  return message_.c_str();
}

inline void WsException::Throw() const { throw *this; }

inline std::string WsException::AppendDetails(const std::string& details) {
  return details.empty() ? "" : ": " + details;
}
}  // namespace ws
