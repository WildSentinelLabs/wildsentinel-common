#include "ws/status/status_code.h"

namespace ws {
std::string StatusCodeToString(StatusCode code) {
  switch (code) {
    case StatusCode::kOk:
      return "OK";
    case StatusCode::kBadRequest:
      return "BAD_REQUEST";
    case StatusCode::kUnauthorized:
      return "UNAUTHORIZED";
    case StatusCode::kForbidden:
      return "FORBIDDEN";
    case StatusCode::kNotFound:
      return "NOT_FOUND";
    case StatusCode::kConflict:
      return "CONFLICT";
    case StatusCode::kInternalError:
      return "INTERNAL_ERROR";
    case StatusCode::kNotImplemented:
      return "NOT_IMPLEMENTED";
    case StatusCode::kRequestAborted:
      return "REQUEST_ABORTED";
    case StatusCode::kTimeout:
      return "TIMEOUT";
    case StatusCode::kPreconditionFailed:
      return "PRECONDITION_FAILED";
    case StatusCode::kPayloadTooLarge:
      return "PAYLOAD_TOO_LARGE";
    case StatusCode::kUnsupported:
      return "UNSUPPORTED";
    case StatusCode::kFailedDependency:
      return "FAILED_DEPENDENCY";
    case StatusCode::kUnavailable:
      return "UNAVAILABLE";
    case StatusCode::kInsufficientStorage:
      return "INSUFFICIENT_STORAGE";
    case StatusCode::kOutOfRange:
      return "OUT_OF_RANGE";
    case StatusCode::kBadAlloc:
      return "BAD_ALLOC";
    case StatusCode::kRuntimeError:
      return "RUNTIME_ERROR";
    case StatusCode::kDataLoss:
      return "DATA_LOSS";
    default:
    case StatusCode::kUnknown:
      return "UNKNOWN_ERROR";
  }
}
}  // namespace ws
