from dhquant.core import dhquant_cpp_binding as _cpp

from .base import BaseNode, PipelineContext


class StrategyPipeline:
    def __init__(self, engine: _cpp.Engine):
        self.engine = engine
        self.nodes: list[BaseNode] = []
        self.context = PipelineContext(engine=engine)

    def add_node(self, node: BaseNode):
        self.nodes.append(node)

    def run(self, bar: _cpp.Bar):
        self.context.current_bar = bar
        self.context.current_time = bar.ts_event
        self.context.pending_orders.clear()

        for node in self.nodes:
            node.process(self.context)

        # 提交所有 Node 产生的订单
        for order in self.context.pending_orders:
            env = _cpp.EventEnvelope()
            env.event_type = _cpp.EventType.ORDER
            env.source = _cpp.EventSource.MANUAL
            env.ts_event = bar.ts_event
            env.payload = order
            self.engine.submit(env)
