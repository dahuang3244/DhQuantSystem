#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace dhquant {

enum class InstrumentType : std::uint8_t {
  kUnknown = 0,
  kStock = 1,
  kEtf = 2,
  kIndex = 3,
};

enum class Side : std::uint8_t {
  kUnknown = 0,
  kBuy = 1,
  kSell = 2,
};

enum class Offset : std::uint8_t {
  kUnknown = 0,
  kOpen = 1,
  kClose = 2,
  kCloseToday = 3,
  kCloseYesterday = 4,
};

enum class OrderType : std::uint8_t {
  kUnknown = 0,
  kLimit = 1,
  kMarket = 2,
};

enum class OrderStatus : std::uint8_t {
  kUnknown = 0,
  kPendingNew = 1,
  kNew = 2,
  kPartiallyFilled = 3,
  kFilled = 4,
  kCancelPending = 5,
  kCancelled = 6,
  kRejected = 7,
};

enum class RejectReason : std::uint8_t {
  kNone = 0,
  kInvalidInstrument = 1,
  kInvalidPrice = 2,
  kInvalidQuantity = 3,
  kRiskRejected = 4,
  kGatewayRejected = 5,
  kUnknown = 255,
};

enum class RuntimeMode : std::uint8_t {
  kUnknown = 0,
  kBacktest = 1,
  kPaper = 2,
  kLive = 3,
};

struct PriceLevel {
  double price{0.0};
  std::int64_t quantity{0};
};

struct Instrument {
  std::string instrument_id;
  std::string exchange;
  std::string symbol;
  InstrumentType instrument_type{InstrumentType::kUnknown};
  std::int64_t lot_size{0};
  double price_tick{0.0};
  std::string currency{"CNY"};
};

struct Tick {
  std::string instrument_id;
  std::int64_t ts_event{0};
  std::int64_t ts_process{0};
  double last_price{0.0};
  std::int64_t last_quantity{0};
  std::vector<PriceLevel> bids;
  std::vector<PriceLevel> asks;
};

struct Bar {
  std::string instrument_id;
  std::int64_t ts_event{0};
  std::int64_t ts_process{0};
  double open{0.0};
  double high{0.0};
  double low{0.0};
  double close{0.0};
  std::int64_t volume{0};
  double turnover{0.0};
};

struct OrderIntent {
  std::string instrument_id;
  Side side{Side::kUnknown};
  Offset offset{Offset::kUnknown};
  OrderType order_type{OrderType::kUnknown};
  std::int64_t quantity{0};
  double price{0.0};
};

struct Order {
  std::string session_id;
  std::string order_id;
  std::string instrument_id;
  Side side{Side::kUnknown};
  Offset offset{Offset::kUnknown};
  OrderType order_type{OrderType::kUnknown};
  OrderStatus status{OrderStatus::kUnknown};
  RejectReason reject_reason{RejectReason::kNone};
  std::int64_t quantity{0};
  std::int64_t filled_quantity{0};
  double price{0.0};
  double average_fill_price{0.0};
  std::int64_t ts_event{0};
  std::int64_t ts_process{0};
};

struct Trade {
  std::string session_id;
  std::string order_id;
  std::string trade_id;
  std::string instrument_id;
  Side side{Side::kUnknown};
  Offset offset{Offset::kUnknown};
  std::int64_t fill_quantity{0};
  double fill_price{0.0};
  double commission{0.0};
  std::int64_t ts_event{0};
  std::int64_t ts_process{0};
};

struct RiskEvent {
  std::string order_id;
  std::string instrument_id;
  bool passed{false};
  RejectReason reason{RejectReason::kNone};
  std::string detail;
  std::int64_t ts_event{0};
  std::int64_t ts_process{0};
};

struct Position {
  std::string instrument_id;
  std::int64_t quantity_total{0};
  std::int64_t quantity_available{0};
  double average_price{0.0};
  double market_value{0.0};
};

struct Account {
  std::string account_id;
  std::string currency{"CNY"};
  double cash{0.0};
  double frozen_cash{0.0};
  double equity{0.0};
};

struct PortfolioSnapshot {
  std::string snapshot_id;
  std::string account_id;
  Account account_state;
  std::vector<Position> positions;
  std::int64_t ts_snapshot{0};
};

std::string_view to_string(InstrumentType value) noexcept;
std::string_view to_string(Side value) noexcept;
std::string_view to_string(Offset value) noexcept;
std::string_view to_string(OrderType value) noexcept;
std::string_view to_string(OrderStatus value) noexcept;
std::string_view to_string(RejectReason value) noexcept;
std::string_view to_string(RuntimeMode value) noexcept;

bool is_terminal(OrderStatus status) noexcept;

} // namespace dhquant
