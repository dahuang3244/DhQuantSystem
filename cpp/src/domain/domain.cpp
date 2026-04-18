#include "dhquant/domain.h"

namespace dhquant {

std::string_view to_string(const InstrumentType value) noexcept {
  switch (value) {
  case InstrumentType::kUnknown:
    return "unknown";
  case InstrumentType::kStock:
    return "stock";
  case InstrumentType::kEtf:
    return "etf";
  case InstrumentType::kIndex:
    return "index";
  }

  return "unknown";
}

std::string_view to_string(const Side value) noexcept {
  switch (value) {
  case Side::kUnknown:
    return "unknown";
  case Side::kBuy:
    return "buy";
  case Side::kSell:
    return "sell";
  }

  return "unknown";
}

std::string_view to_string(const Offset value) noexcept {
  switch (value) {
  case Offset::kUnknown:
    return "unknown";
  case Offset::kOpen:
    return "open";
  case Offset::kClose:
    return "close";
  case Offset::kCloseToday:
    return "close_today";
  case Offset::kCloseYesterday:
    return "close_yesterday";
  }

  return "unknown";
}

std::string_view to_string(const OrderType value) noexcept {
  switch (value) {
  case OrderType::kUnknown:
    return "unknown";
  case OrderType::kLimit:
    return "limit";
  case OrderType::kMarket:
    return "market";
  }

  return "unknown";
}

std::string_view to_string(const OrderStatus value) noexcept {
  switch (value) {
  case OrderStatus::kUnknown:
    return "unknown";
  case OrderStatus::kPendingNew:
    return "pending_new";
  case OrderStatus::kNew:
    return "new";
  case OrderStatus::kPartiallyFilled:
    return "partially_filled";
  case OrderStatus::kFilled:
    return "filled";
  case OrderStatus::kCancelPending:
    return "cancel_pending";
  case OrderStatus::kCancelled:
    return "cancelled";
  case OrderStatus::kRejected:
    return "rejected";
  }

  return "unknown";
}

std::string_view to_string(const RejectReason value) noexcept {
  switch (value) {
  case RejectReason::kNone:
    return "none";
  case RejectReason::kInvalidInstrument:
    return "invalid_instrument";
  case RejectReason::kInvalidPrice:
    return "invalid_price";
  case RejectReason::kInvalidQuantity:
    return "invalid_quantity";
  case RejectReason::kRiskRejected:
    return "risk_rejected";
  case RejectReason::kGatewayRejected:
    return "gateway_rejected";
  case RejectReason::kUnknown:
    return "unknown";
  }

  return "unknown";
}

std::string_view to_string(const RuntimeMode value) noexcept {
  switch (value) {
  case RuntimeMode::kUnknown:
    return "unknown";
  case RuntimeMode::kBacktest:
    return "backtest";
  case RuntimeMode::kPaper:
    return "paper";
  case RuntimeMode::kLive:
    return "live";
  }

  return "unknown";
}

bool is_terminal(const OrderStatus status) noexcept {
  return status == OrderStatus::kFilled || status == OrderStatus::kCancelled ||
         status == OrderStatus::kRejected;
}

} // namespace dhquant
