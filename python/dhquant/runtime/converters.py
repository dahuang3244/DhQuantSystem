# python/dhquant/runtime/converters.py
"""
对象转换层。

把 Python dataclass <-> binding 原生对象之间的转换集中在这里管理。
遵守一个原则：显式转换，不做魔法。
"""
from __future__ import annotations

from dhquant import domain
from dhquant.core import dhquant_cpp_binding as _cpp

_SIDE_TO_DOMAIN = {
    _cpp.Side.UNKNOWN: domain.Side.UNKNOWN,
    _cpp.Side.BUY: domain.Side.BUY,
    _cpp.Side.SELL: domain.Side.SELL,
}
_SIDE_TO_CPP = {value: key for key, value in _SIDE_TO_DOMAIN.items()}

_OFFSET_TO_DOMAIN = {
    _cpp.Offset.UNKNOWN: domain.Offset.UNKNOWN,
    _cpp.Offset.OPEN: domain.Offset.OPEN,
    _cpp.Offset.CLOSE: domain.Offset.CLOSE,
    _cpp.Offset.CLOSE_TODAY: domain.Offset.CLOSE_TODAY,
    _cpp.Offset.CLOSE_YESTERDAY: domain.Offset.CLOSE_YESTERDAY,
}
_OFFSET_TO_CPP = {value: key for key, value in _OFFSET_TO_DOMAIN.items()}

_ORDER_TYPE_TO_DOMAIN = {
    _cpp.OrderType.UNKNOWN: domain.OrderType.UNKNOWN,
    _cpp.OrderType.LIMIT: domain.OrderType.LIMIT,
    _cpp.OrderType.MARKET: domain.OrderType.MARKET,
}
_ORDER_TYPE_TO_CPP = {value: key for key, value in _ORDER_TYPE_TO_DOMAIN.items()}

_ORDER_STATUS_TO_DOMAIN = {
    _cpp.OrderStatus.UNKNOWN: domain.OrderStatus.UNKNOWN,
    _cpp.OrderStatus.PENDING_NEW: domain.OrderStatus.PENDING_NEW,
    _cpp.OrderStatus.NEW: domain.OrderStatus.NEW,
    _cpp.OrderStatus.PARTIALLY_FILLED: domain.OrderStatus.PARTIALLY_FILLED,
    _cpp.OrderStatus.FILLED: domain.OrderStatus.FILLED,
    _cpp.OrderStatus.CANCEL_PENDING: domain.OrderStatus.CANCEL_PENDING,
    _cpp.OrderStatus.CANCELLED: domain.OrderStatus.CANCELLED,
    _cpp.OrderStatus.REJECTED: domain.OrderStatus.REJECTED,
}

_REJECT_REASON_TO_DOMAIN = {
    _cpp.RejectReason.NONE: domain.RejectReason.NONE,
    _cpp.RejectReason.INVALID_INSTRUMENT: domain.RejectReason.INVALID_INSTRUMENT,
    _cpp.RejectReason.INVALID_PRICE: domain.RejectReason.INVALID_PRICE,
    _cpp.RejectReason.INVALID_QUANTITY: domain.RejectReason.INVALID_QUANTITY,
    _cpp.RejectReason.RISK_REJECTED: domain.RejectReason.RISK_REJECTED,
    _cpp.RejectReason.GATEWAY_REJECTED: domain.RejectReason.GATEWAY_REJECTED,
    _cpp.RejectReason.UNKNOWN: domain.RejectReason.UNKNOWN,
}

# ── Tick 转换 ─────────────────────────────────────────────────


def tick_from_binding(raw: _cpp.Tick) -> domain.Tick:
    """
    把 binding 侧的 Tick 对象转换成 Python dataclass Tick。

    为什么需要这个？
    C++ Core 传出来的是原生对象，策略想要的是 dataclass（方便 .field 读取）。
    """
    return domain.Tick(
        instrument_id=raw.instrument_id,
        ts_event=raw.ts_event,
        ts_process=raw.ts_process,
        last_price=raw.last_price,
        last_quantity=raw.last_quantity,
        bids=[domain.PriceLevel(price=b.price, quantity=b.quantity) for b in raw.bids],
        asks=[domain.PriceLevel(price=a.price, quantity=a.quantity) for a in raw.asks],
    )


# ── Bar 转换 ──────────────────────────────────────────────────


def bar_from_binding(raw: _cpp.Bar) -> domain.Bar:
    """把 binding Bar -> Python dataclass Bar。"""
    return domain.Bar(
        instrument_id=raw.instrument_id,
        ts_event=raw.ts_event,
        ts_process=raw.ts_process,
        open=raw.open,
        high=raw.high,
        low=raw.low,
        close=raw.close,
        volume=raw.volume,
        turnover=raw.turnover,
    )


# ── Order 转换 ────────────────────────────────────────────────


def order_from_binding(raw: _cpp.Order) -> domain.Order:
    """把 binding Order -> Python dataclass Order。"""
    return domain.Order(
        session_id=raw.session_id,
        order_id=raw.order_id,
        instrument_id=raw.instrument_id,
        side=_SIDE_TO_DOMAIN[raw.side],
        offset=_OFFSET_TO_DOMAIN[raw.offset],
        order_type=_ORDER_TYPE_TO_DOMAIN[raw.order_type],
        status=_ORDER_STATUS_TO_DOMAIN[raw.status],
        reject_reason=_REJECT_REASON_TO_DOMAIN[raw.reject_reason],
        quantity=raw.quantity,
        filled_quantity=raw.filled_quantity,
        price=raw.price,
        average_fill_price=raw.average_fill_price,
        ts_event=raw.ts_event,
        ts_process=raw.ts_process,
    )


def order_to_binding(order: domain.Order) -> _cpp.Order:
    """
    把 Python dataclass Order 转换成 binding 原生 Order。

    这用于 submit_order 时，把策略发出的 Python 订单送进 C++ Core。
    """
    raw = _cpp.Order()
    raw.session_id = order.session_id
    raw.order_id = order.order_id
    raw.instrument_id = order.instrument_id
    raw.side = _SIDE_TO_CPP[order.side]
    raw.offset = _OFFSET_TO_CPP[order.offset]
    raw.order_type = _ORDER_TYPE_TO_CPP[order.order_type]
    raw.status = next(
        key for key, value in _ORDER_STATUS_TO_DOMAIN.items() if value is order.status
    )
    raw.reject_reason = next(
        key for key, value in _REJECT_REASON_TO_DOMAIN.items() if value is order.reject_reason
    )
    raw.quantity = order.quantity
    raw.filled_quantity = order.filled_quantity
    raw.price = order.price
    raw.average_fill_price = order.average_fill_price
    raw.ts_event = order.ts_event
    raw.ts_process = order.ts_process
    return raw


def order_intent_to_binding(intent: domain.OrderIntent) -> _cpp.OrderIntent:
    raw = _cpp.OrderIntent()
    raw.instrument_id = intent.instrument_id
    raw.side = _SIDE_TO_CPP[intent.side]
    raw.offset = _OFFSET_TO_CPP[intent.offset]
    raw.order_type = _ORDER_TYPE_TO_CPP[intent.order_type]
    raw.quantity = intent.quantity
    raw.price = intent.price
    return raw


# ── Trade 转换 ────────────────────────────────────────────────


def trade_from_binding(raw: _cpp.Trade) -> domain.Trade:
    """把 binding Trade -> Python dataclass Trade。"""
    return domain.Trade(
        session_id=raw.session_id,
        order_id=raw.order_id,
        trade_id=raw.trade_id,
        instrument_id=raw.instrument_id,
        side=_SIDE_TO_DOMAIN[raw.side],
        offset=_OFFSET_TO_DOMAIN[raw.offset],
        fill_quantity=raw.fill_quantity,
        fill_price=raw.fill_price,
        commission=raw.commission,
        ts_event=raw.ts_event,
        ts_process=raw.ts_process,
    )


def portfolio_snapshot_from_binding(raw: _cpp.PortfolioSnapshot) -> domain.PortfolioSnapshot:
    return domain.PortfolioSnapshot(
        snapshot_id=raw.snapshot_id,
        account_id=raw.account_id,
        account_state=domain.Account(
            account_id=raw.account_state.account_id,
            currency=raw.account_state.currency,
            cash=raw.account_state.cash,
            frozen_cash=raw.account_state.frozen_cash,
            equity=raw.account_state.equity,
        ),
        positions=[
            domain.Position(
                instrument_id=position.instrument_id,
                quantity_total=position.quantity_total,
                quantity_available=position.quantity_available,
                average_price=position.average_price,
                market_value=position.market_value,
            )
            for position in raw.positions
        ],
        ts_snapshot=raw.ts_snapshot,
    )


# ── Engine Status 视图 ────────────────────────────────────────


def engine_status_to_dict(status: _cpp.EngineStatus) -> dict:
    """
    把 EngineStatus 转换成 Python dict，方便日志 and 调试。
    """
    return {
        "mode": str(status.mode),
        "running": status.running,
        "state": str(status.state),
        "last_error": status.last_error,
    }
