#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utils/ErrorCode.h>
#include <variant>

namespace dhquant {
namespace Result {

struct Error {
  ErrorCode::ErrorCode code{ErrorCode::ErrorCode::kOk};
  std::string message;
  std::unordered_map<std::string, std::string> context;

  bool ok() const noexcept { return code == ErrorCode::ErrorCode::kOk; }
};

template <typename T> class Result {
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

  [[nodiscard]] bool ok() const noexcept {
    return std::holds_alternative<T>(data_);
  }

  [[nodiscard]] const T &value() const & {
    if (!ok())
      throw std::runtime_error("Accessing value of an error Result: " +
                               error().message);
    return std::get<T>(data_);
  }

  [[nodiscard]] T &&value() && {
    if (!ok())
      throw std::runtime_error("Accessing value of an error Result");
    return std::get<T>(std::move(data_));
  }

  [[nodiscard]] const Error &error() const noexcept {
    if (ok()) {
      static Error kNoError{ErrorCode::ErrorCode::kOk, "No error"};
      return kNoError;
    }
    return std::get<Error>(data_);
  }

private:
  std::variant<T, Error> data_;
};

// Result<void> 特化
template <> class Result<void> {
public:
  Result() : error_({ErrorCode::ErrorCode::kOk, ""}) {}
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

} // namespace Result
} // namespace dhquant