# python/dhquant/runtime/runner.py
"""
StrategyRunner：策略生命周期编排入口。

职责：
  - 创建并启动 C++ Engine
  - 创建 StrategyContext
  - 触发 Strategy 生命周期回调
  - 处理 Python 侧异常
  - 在停止时确保 on_stop 和 engine.stop() 被调用

第一版：
  - 使用 mock 事件循环（手动推入假数据）
  - Phase 4 之后换成真实 EventLoop 驱动
"""
from __future__ import annotations

import logging
from dataclasses import dataclass

from dhquant import domain
from dhquant.core import dhquant_cpp_binding as _cpp
from dhquant.runtime.context import StrategyContext
from dhquant.runtime.strategy import Strategy

logger = logging.getLogger(__name__)


@dataclass
class RunnerConfig:
    """Runner 的启动配置。"""

    mode: domain.RuntimeMode = domain.RuntimeMode.BACKTEST
    name: str = "dhquant_strategy"


class StrategyRunner:
    """
    策略运行时编排器。

    参数：
    - config: RunnerConfig，指定运行模式等参数
    """

    def __init__(self, config: RunnerConfig | None = None) -> None:
        self._config = config or RunnerConfig()

    def run(self, strategy: Strategy) -> None:
        """
        启动策略，进入运行主循环，直到停止。

        第一版使用 mock 主循环，后续替换为真实事件驱动。
        """
        # 1. 把 Python RuntimeMode 转成 binding RuntimeMode
        mode_map = {
            domain.RuntimeMode.BACKTEST: _cpp.RuntimeMode.BACKTEST,
            domain.RuntimeMode.PAPER: _cpp.RuntimeMode.PAPER,
            domain.RuntimeMode.LIVE: _cpp.RuntimeMode.LIVE,
        }
        cpp_mode = mode_map.get(self._config.mode, _cpp.RuntimeMode.BACKTEST)

        # 2. 创建并启动 Engine
        engine = _cpp.Engine(cpp_mode)
        engine.start()
        logger.info(
            "Engine started", extra={"mode": self._config.mode, "status": engine.status().running}
        )

        # 3. 创建 StrategyContext
        ctx = StrategyContext(engine)

        try:
            # 4. 触发 on_init
            strategy.on_init(ctx)

            # 5. 第一版：mock 主循环（直接结束，等 Phase 4 替换）
            logger.info("Mock event loop: no real events in Phase 3 first version.")
            self._mock_loop(strategy, ctx)

        except Exception as e:
            logger.exception("Strategy raised unhandled exception", exc_info=e)
        finally:
            # 6. 无论如何都调 on_stop 和 engine.stop()
            try:
                strategy.on_stop(ctx)
            except Exception as e:
                logger.exception("on_stop raised exception", exc_info=e)
            engine.stop()
            logger.info("Engine stopped")

    def _mock_loop(self, strategy: Strategy, ctx: StrategyContext) -> None:
        """
        Mock 事件循环。

        第一版：什么事件都不推，只是让结构成立。
        Phase 4 里这里会换成真实的 EventLoop 驱动。
        """
        logger.info(
            "Mock loop: no events to push. Strategy on_init called and on_stop will follow."
        )
        # 未来替换为：
        # for event in engine.poll_events():
        #     if event is Tick:
        #         strategy.on_tick(ctx, converters.tick_from_binding(event))
        #     elif event is Bar:
        #         strategy.on_bar(ctx, converters.bar_from_binding(event))
        #     ...
