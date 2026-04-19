# python/dhquant/strategy/samples/mock_strategy.py
"""
MockStrategy：用于 smoke test 的最简策略实现。

它验证：
  - Strategy 基类可以被继承
  - on_init / on_stop 能被正常调用
  - ctx.log / ctx.now / ctx.query_state 能正常工作

不做任何真实交易逻辑。
"""
from __future__ import annotations

import logging

from dhquant.runtime.context import StrategyContext
from dhquant.runtime.strategy import Strategy

logger = logging.getLogger(__name__)


class MockStrategy(Strategy):
    """最简 Mock 策略，用于验证 Runtime 框架是否能跑通。"""

    def on_init(self, ctx: StrategyContext) -> None:
        logger.info("[MockStrategy] on_init called")
        logger.info(f"  Engine status: {ctx.query_state().running}")
        logger.info(f"  Current time (ns): {ctx.now()}")
        ctx.log("INFO", "MockStrategy initialized successfully")

    def on_stop(self, ctx: StrategyContext) -> None:
        logger.info("[MockStrategy] on_stop called. Strategy is stopping.")
