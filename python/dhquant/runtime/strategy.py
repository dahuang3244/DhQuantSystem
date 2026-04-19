# python/dhquant/runtime/strategy.py
"""
Strategy 基类。

策略作者继承这个类，重写感兴趣的回调方法即可。
不需要重写全部方法——默认实现都是 pass，不会报错。
"""
from __future__ import annotations

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from dhquant import domain
    from dhquant.runtime.context import StrategyContext


class Strategy:
    """策略基类。继承它，重写你需要的回调方法。"""

    def on_init(self, ctx: StrategyContext) -> None:
        """Engine 启动后调用一次。适合做初始化：订阅品种、初始化状态变量。"""
        pass

    def on_tick(self, ctx: StrategyContext, tick: domain.Tick) -> None:
        """每次 Tick 行情到达时调用。"""
        pass

    def on_bar(self, ctx: StrategyContext, bar: domain.Bar) -> None:
        """每次 K 线数据到达时调用。"""
        pass

    def on_order(self, ctx: StrategyContext, order: domain.Order) -> None:
        """订单状态发生变化时调用（新建、成交、拒单、取消等）。"""
        pass

    def on_trade(self, ctx: StrategyContext, trade: domain.Trade) -> None:
        """成交发生时调用。"""
        pass

    def on_stop(self, ctx: StrategyContext) -> None:
        """策略停止前调用。适合做清理：保存状态、打印汇总。"""
        pass
