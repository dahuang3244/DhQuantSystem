"""
dhquant.runtime
--------------
Python 侧策略运行时门面。

包含：
- Strategy  : 策略基类，策略作者继承它
- StrategyContext : 策略访问引擎能力的统一入口
- StrategyRunner  : 生命周期编排入口，负责启动和停止
- converters      : Python dataclass <-> binding 边界对象的转换工具

使用方式：
    from dhquant.runtime import Strategy, StrategyContext, StrategyRunner
"""

from dhquant.runtime.context import StrategyContext
from dhquant.runtime.runner import StrategyRunner
from dhquant.runtime.strategy import Strategy

__all__ = ["Strategy", "StrategyContext", "StrategyRunner"]
