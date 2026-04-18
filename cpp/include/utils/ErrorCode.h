#pragma once

#include <string>
#include <string_view>

namespace ErrorCode {

enum class ErrorCode {
  kOk = 0,
  kInvalidArgument = 1,
  kNotFound = 2,
  kIoError = 3,
  kTimeout = 4,
  kStateError = 5,
  kInternalError = 6,
  kUnknownError = 7
};

inline std::string ErrorCodeToString(ErrorCode code) {
  switch (code) {
  case ErrorCode::kOk:
    return "OK";
  case ErrorCode::kInvalidArgument:
    return "INVALID_ARGUMENT";
  case ErrorCode::kNotFound:
    return "NOT_FOUND";
  case ErrorCode::kIoError:
    return "IO_ERROR";
  case ErrorCode::kTimeout:
    return "TIMEOUT";
  case ErrorCode::kStateError:
    return "STATE_ERROR";
  case ErrorCode::kInternalError:
    return "INTERNAL_ERROR";
  default:
    return "UNKNOWN_ERROR";
  }
}

} // namespace ErrorCode
