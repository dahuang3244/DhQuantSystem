"""Python runtime public surface with lazy imports to avoid circular dependencies."""

__all__ = ["Strategy", "StrategyContext", "StrategyRunner"]


def __getattr__(name: str):
    if name == "Strategy":
        from dhquant.runtime.strategy import Strategy

        return Strategy
    if name == "StrategyContext":
        from dhquant.runtime.context import StrategyContext

        return StrategyContext
    if name == "StrategyRunner":
        from dhquant.runtime.runner import StrategyRunner

        return StrategyRunner
    raise AttributeError(name)
