#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utils/ErrorCode.h>
#include <variant>

namespace dhquant {

struct Error {
  ErrorCode code{ErrorCode::kOk};
  std::string message;
  std::unordered_map<std::string, std::string> context;

  bool ok() const noexcept { return code == ErrorCode::kOk; }

  // 静态辅助方法：快速创建常见类型的错误
  static Error Invalid(std::string msg) {
    return {ErrorCode::kInvalidArgument, std::move(msg)};
  }
  static Error NotFound(std::string msg) {
    return {ErrorCode::kNotFound, std::move(msg)};
  }
  static Error Internal(std::string msg) {
    return {ErrorCode::kInternalError, std::move(msg)};
  }
  static Error State(std::string msg) {
    return {ErrorCode::kStateError, std::move(msg)};
  }
};

template <typename T> class [[nodiscard]] Result {
public:
  // 构造函数
  Result(const T &value) : data_(value) {}
  Result(T &&value) : data_(std::move(value)) {}
  Result(const Error &error) : data_(error) {}
  Result(Error &&error) : data_(std::move(error)) {}

  static Result<T> Ok(T value) noexcept { return Result(std::move(value)); }
  static Result<T> Err(Error error) noexcept {
    return Result(std::move(error));
  }

  bool ok() const noexcept { return std::holds_alternative<T>(data_); }

  const T &value() const & {
    if (!ok())
      throw std::runtime_error("Accessing value of an error Result: " +
                               error().message);
    return std::get<T>(data_);
  }

  T &&value() && {
    if (!ok())
      throw std::runtime_error("Accessing value of an error Result");
    return std::get<T>(std::move(data_));
  }

  const Error &error() const noexcept {
    if (ok()) {
      static Error kNoError{ErrorCode::kOk, "No error"};
      return kNoError;
    }
    return std::get<Error>(data_);
  }

private:
  std::variant<T, Error> data_;
};

// Result<void> 特化
template <> class [[nodiscard]] Result<void> {
public:
  Result() : error_({ErrorCode::kOk, ""}) {}
  Result(const Error &error) : error_(error) {}

  static Result<void> Ok() noexcept { return Result(); }
  static Result<void> Err(Error error) noexcept {
    return Result(std::move(error));
  }

  bool ok() const noexcept { return error_.ok(); }
  const Error &error() const noexcept { return error_; }

private:
  Error error_;
};

// 简化宏
#define DH_OK() dhquant::Result<void>::Ok()
#define DH_ERR(code, msg)                                                      \
  dhquant::Error { code, msg }
#define DH_OK_V(val) dhquant::Result<decltype(val)>::Ok(val)

} // namespace dhquant