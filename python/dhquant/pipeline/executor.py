from dhquant.core import dhquant_cpp_binding as _cpp

from .pipeline import StrategyPipeline


class PipelineExecutor:
    def __init__(self, pipeline: StrategyPipeline):
        self.pipeline = pipeline

    def on_bar(self, bar: _cpp.Bar):
        self.pipeline.run(bar)

    def on_order(self, order: _cpp.Order):
        # 可以在这里更新状态或记录
        pass

    def on_trade(self, trade: _cpp.Trade):
        # 可以在这里记录成交
        pass
