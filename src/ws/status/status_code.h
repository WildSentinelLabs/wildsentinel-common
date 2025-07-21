#pragma once
#include <string>

namespace ws {
enum class StatusCode {
  kOk,
  kBadRequest,
  kUnauthorized,
  kForbidden,
  kNotFound,
  kConflict,
  kInternalError,
  kNotImplemented,
  kRequestAborted,
  kTimeout,
  kPreconditionFailed,
  kPayloadTooLarge,
  kUnsupported,
  kFailedDependency,
  kUnavailable,
  kInsufficientStorage,
  kOutOfRange,
  kBadAlloc,
  kRuntimeError,
  kDataLoss,
  kUnknown,
};

std::string StatusCodeToString(StatusCode code);
}  // namespace ws
