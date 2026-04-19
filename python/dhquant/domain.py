from __future__ import annotations

from dataclasses import dataclass, field
from enum import Enum


class InstrumentType(str, Enum):
    UNKNOWN = "unknown"
    STOCK = "stock"
    ETF = "etf"
    INDEX = "index"


class Side(str, Enum):
    UNKNOWN = "unknown"
    BUY = "buy"
    SELL = "sell"


class Offset(str, Enum):
    UNKNOWN = "unknown"
    OPEN = "open"
    CLOSE = "close"
    CLOSE_TODAY = "close_today"
    CLOSE_YESTERDAY = "close_yesterday"


class OrderType(str, Enum):
    UNKNOWN = "unknown"
    LIMIT = "limit"
    MARKET = "market"


class OrderStatus(str, Enum):
    UNKNOWN = "unknown"
    PENDING_NEW = "pending_new"
    NEW = "new"
    PARTIALLY_FILLED = "partially_filled"
    FILLED = "filled"
    CANCEL_PENDING = "cancel_pending"
    CANCELLED = "cancelled"
    REJECTED = "rejected"


class RejectReason(str, Enum):
    NONE = "none"
    INVALID_INSTRUMENT = "invalid_instrument"
    INVALID_PRICE = "invalid_price"
    INVALID_QUANTITY = "invalid_quantity"
    RISK_REJECTED = "risk_rejected"
    GATEWAY_REJECTED = "gateway_rejected"
    UNKNOWN = "unknown"


class RuntimeMode(str, Enum):
    UNKNOWN = "unknown"
    BACKTEST = "backtest"
    PAPER = "paper"
    LIVE = "live"


@dataclass
class PriceLevel:
    price: float = 0.0
    quantity: int = 0


@dataclass
class Instrument:
    instrument_id: str = ""
    exchange: str = ""
    symbol: str = ""
    instrument_type: InstrumentType = InstrumentType.UNKNOWN
    lot_size: int = 0
    price_tick: float = 0.0
    currency: str = "CNY"


@dataclass
class Tick:
    instrument_id: str = ""
    ts_event: int = 0
    ts_process: int = 0
    last_price: float = 0.0
    last_quantity: int = 0
    bids: list[PriceLevel] = field(default_factory=list)
    asks: list[PriceLevel] = field(default_factory=list)


@dataclass
class Bar:
    instrument_id: str = ""
    ts_event: int = 0
    ts_process: int = 0
    open: float = 0.0
    high: float = 0.0
    low: float = 0.0
    close: float = 0.0
    volume: int = 0
    turnover: float = 0.0


@dataclass
class OrderIntent:
    instrument_id: str = ""
    side: Side = Side.UNKNOWN
    offset: Offset = Offset.UNKNOWN
    order_type: OrderType = OrderType.UNKNOWN
    quantity: int = 0
    price: float = 0.0


@dataclass
class Order:
    session_id: str = ""
    order_id: str = ""
    instrument_id: str = ""
    side: Side = Side.UNKNOWN
    offset: Offset = Offset.UNKNOWN
    order_type: OrderType = OrderType.UNKNOWN
    status: OrderStatus = OrderStatus.UNKNOWN
    reject_reason: RejectReason = RejectReason.NONE
    quantity: int = 0
    filled_quantity: int = 0
    price: float = 0.0
    average_fill_price: float = 0.0
    ts_event: int = 0
    ts_process: int = 0


@dataclass
class Trade:
    session_id: str = ""
    order_id: str = ""
    trade_id: str = ""
    instrument_id: str = ""
    side: Side = Side.UNKNOWN
    offset: Offset = Offset.UNKNOWN
    fill_quantity: int = 0
    fill_price: float = 0.0
    commission: float = 0.0
    ts_event: int = 0
    ts_process: int = 0


@dataclass
class RiskEvent:
    order_id: str = ""
    instrument_id: str = ""
    passed: bool = False
    reason: RejectReason = RejectReason.NONE
    detail: str = ""
    ts_event: int = 0
    ts_process: int = 0


@dataclass
class Position:
    instrument_id: str = ""
    quantity_total: int = 0
    quantity_available: int = 0
    average_price: float = 0.0
    market_value: float = 0.0


@dataclass
class Account:
    account_id: str = ""
    currency: str = "CNY"
    cash: float = 0.0
    frozen_cash: float = 0.0
    equity: float = 0.0


@dataclass
class PortfolioSnapshot:
    snapshot_id: str = ""
    account_id: str = ""
    account_state: Account = field(default_factory=Account)
    positions: list[Position] = field(default_factory=list)
    ts_snapshot: int = 0


def is_terminal(status: OrderStatus) -> bool:
    return status in {
        OrderStatus.FILLED,
        OrderStatus.CANCELLED,
        OrderStatus.REJECTED,
    }
