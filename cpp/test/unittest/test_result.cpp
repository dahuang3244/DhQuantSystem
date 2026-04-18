#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <utils/ErrorCode.h>
#include <utils/Result.hpp>

namespace {

using dhquant::Result::Error;
using dhquant::Result::Result;

TEST(ResultTest, OkCarriesValue) {
  auto result = Result<int>::Ok(42);

  EXPECT_TRUE(result.ok());
  EXPECT_EQ(result.value(), 42);
  EXPECT_TRUE(result.error().ok());
}

TEST(ResultTest, ErrCarriesError) {
  Error error{ErrorCode::ErrorCode::kInvalidArgument, "bad argument"};
  auto result = Result<int>::Err(error);

  EXPECT_FALSE(result.ok());
  EXPECT_EQ(result.error().code, ErrorCode::ErrorCode::kInvalidArgument);
  EXPECT_EQ(result.error().message, "bad argument");
}

TEST(ResultTest, AccessingValueOnErrorThrows) {
  auto result = Result<std::string>::Err(
      Error{ErrorCode::ErrorCode::kStateError, "not ready"});

  EXPECT_THROW(static_cast<void>(result.value()), std::runtime_error);
}

TEST(ResultTest, ErrorOnOkReturnsDefaultNoError) {
  auto result = Result<std::string>::Ok("ready");

  EXPECT_TRUE(result.error().ok());
  EXPECT_EQ(result.error().code, ErrorCode::ErrorCode::kOk);
  EXPECT_EQ(result.error().message, "No error");
}

TEST(ResultTest, ErrorContextIsPreserved) {
  Error error{
      ErrorCode::ErrorCode::kInternalError,
      "order rejected",
      {{"symbol", "BTCUSDT"}, {"reason", "risk limit"}},
  };

  auto result = Result<int>::Err(std::move(error));

  ASSERT_FALSE(result.ok());
  ASSERT_EQ(result.error().context.size(), 2U);
  EXPECT_EQ(result.error().context.at("symbol"), "BTCUSDT");
  EXPECT_EQ(result.error().context.at("reason"), "risk limit");
}

TEST(ResultTest, RvalueValueMovesPayloadOut) {
  auto result = Result<std::vector<int>>::Ok({1, 2, 3});

  std::vector<int> moved_value = std::move(result).value();

  EXPECT_EQ(moved_value, (std::vector<int>{1, 2, 3}));
}

TEST(ResultTest, ExceptionMessageIncludesErrorMessage) {
  auto result = Result<std::string>::Err(
      Error{ErrorCode::ErrorCode::kStateError, "engine stopped"});

  try {
    static_cast<void>(result.value());
    FAIL() << "expected value() to throw";
  } catch (const std::runtime_error &ex) {
    EXPECT_NE(std::string(ex.what()).find("engine stopped"), std::string::npos);
  }
}

TEST(ResultVoidTest, OkAndErrorStateWork) {
  auto ok_result = Result<void>::Ok();
  auto err_result =
      Result<void>::Err(Error{ErrorCode::ErrorCode::kTimeout, "timed out"});

  EXPECT_TRUE(ok_result.ok());
  EXPECT_FALSE(err_result.ok());
  EXPECT_EQ(err_result.error().code, ErrorCode::ErrorCode::kTimeout);
  EXPECT_EQ(err_result.error().message, "timed out");
}

TEST(ResultVoidTest, ErrorContextIsPreserved) {
  auto result = Result<void>::Err(Error{
      ErrorCode::ErrorCode::kIoError,
      "disk unavailable",
      {{"path", "/tmp/orders"}},
  });

  ASSERT_FALSE(result.ok());
  EXPECT_EQ(result.error().context.at("path"), "/tmp/orders");
}

} // namespace
