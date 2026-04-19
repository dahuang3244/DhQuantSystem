from dhquant.core import dhquant_cpp_binding as _cpp
from dhquant.oms.order_service import OrderService

from .base import BaseNode, PipelineContext


class StrategyPipeline:
    def __init__(self, engine: _cpp.Engine):
        self.engine = engine
        self.nodes: list[BaseNode] = []
        self.order_service = OrderService(engine)
        self.context = PipelineContext(engine=engine, order_service=self.order_service)

    def add_node(self, node: BaseNode):
        self.nodes.append(node)

    def run(self, bar: _cpp.Bar):
        self.context.current_bar = bar
        self.context.current_time = bar.ts_event

        for node in self.nodes:
            node.process(self.context)
