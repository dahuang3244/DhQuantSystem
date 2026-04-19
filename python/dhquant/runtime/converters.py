# python/dhquant/runtime/converters.py
"""
对象转换层。

把 Python dataclass <-> binding 原生对象之间的转换集中在这里管理。
遵守一个原则：显式转换，不做魔法。
"""
from __future__ import annotations

import dhquant_cpp_binding as _cpp

from dhquant import domain

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
    raw.quantity = order.quantity
    raw.price = order.price
    # 注意：side / offset / order_type 当前 binding 里还没导出
    # TODO: 等 binding 补全枚举后，补上这几行
    return raw


# ── Trade 转换 ────────────────────────────────────────────────


def trade_from_binding(raw: _cpp.Trade) -> domain.Trade:
    """把 binding Trade -> Python dataclass Trade。"""
    return domain.Trade(
        session_id=raw.session_id,
        order_id=raw.order_id,
        trade_id=raw.trade_id,
        instrument_id=raw.instrument_id,
        fill_quantity=raw.fill_quantity,
        fill_price=raw.fill_price,
        commission=raw.commission,
        ts_event=raw.ts_event,
        ts_process=raw.ts_process,
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
