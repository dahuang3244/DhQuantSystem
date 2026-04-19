# python/dhquant/runtime/context.py
"""
StrategyContext：策略访问引擎能力的统一门面。

策略代码只通过 Context 发出动作。
它不是业务逻辑层，它只是一个转发器 + 薄薄的适配层。
"""
from __future__ import annotations

import logging
import time
from typing import Any

from dhquant import domain
from dhquant.core import dhquant_cpp_binding as _cpp
from dhquant.oms.order_service import OrderService

logger = logging.getLogger(__name__)


class StrategyContext:
    """
    策略运行时上下文。

    参数：
    - engine: 原生 C++ Engine 对象（由 runner 创建并传入）
    """

    def __init__(self, engine: _cpp.Engine, order_service: OrderService | None = None) -> None:
        self._engine = engine
        self._order_service = order_service or OrderService(engine)

    # ── 动作接口 ──────────────────────────────────────────────

    def submit_order(self, intent: domain.OrderIntent) -> domain.Order:
        """提交下单意图，经由统一的 OrderService 进入 C++ OMS 主链。"""
        logger.info("submit_order called", extra={"instrument_id": intent.instrument_id})
        return self._order_service.submit(intent)

    def cancel_order(self, order_id: str) -> domain.Order:
        """撤销订单。"""
        logger.info("cancel_order called", extra={"order_id": order_id})
        return self._order_service.cancel(order_id)

    # ── 查询接口 ──────────────────────────────────────────────

    def query_state(self) -> _cpp.EngineStatus:
        """
        查询当前引擎状态。

        返回 binding 侧的 EngineStatus，包含 mode / running / state / last_error。
        """
        return self._engine.status()

    def query_portfolio(self) -> domain.PortfolioSnapshot:
        """查询当前账本快照。"""
        return self._order_service.get_portfolio_snapshot()

    # ── 工具接口 ──────────────────────────────────────────────

    def log(self, level: str, message: str, **fields: Any) -> None:
        """
        打日志。

        level: "DEBUG" / "INFO" / "WARNING" / "ERROR"
        fields: 额外的结构化字段
        """
        log_fn = getattr(logger, level.lower(), logger.info)
        log_fn(message, extra=fields)

    def now(self) -> int:
        """
        返回当前时间，Unix 纳秒时间戳。

        第一版使用 Python time 模块。
        后续与 C++ Clock 对齐后，可以改成从 Engine 取时间。
        """
        return time.time_ns()
