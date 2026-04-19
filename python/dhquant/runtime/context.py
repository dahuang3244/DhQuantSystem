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

import dhquant_cpp_binding as _cpp

from dhquant import domain

logger = logging.getLogger(__name__)


class StrategyContext:
    """
    策略运行时上下文。

    参数：
    - engine: 原生 C++ Engine 对象（由 runner 创建并传入）
    """

    def __init__(self, engine: _cpp.Engine) -> None:
        self._engine = engine

    # ── 动作接口 ──────────────────────────────────────────────

    def submit_order(self, order: domain.Order) -> None:
        """
        提交订单。

        第一版实现：打日志占位，等 converters 和 Engine.submit 打通后
        再换成真实的转换 + 提交调用。
        """
        logger.info(
            "submit_order called",
            extra={"order_id": order.order_id, "instrument_id": order.instrument_id},
        )
        # TODO Phase 3 后续：
        # from dhquant.runtime import converters
        # envelope = converters.order_to_event_envelope(order)
        # self._engine.submit(envelope)

    def cancel_order(self, order_id: str) -> None:
        """撤销订单。"""
        logger.info("cancel_order called", extra={"order_id": order_id})
        # TODO Phase 3 后续：构造 cancel 事件并提交

    # ── 查询接口 ──────────────────────────────────────────────

    def query_state(self) -> _cpp.EngineStatus:
        """
        查询当前引擎状态。

        返回 binding 侧的 EngineStatus，包含 mode / running / state / last_error。
        """
        return self._engine.status()

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
