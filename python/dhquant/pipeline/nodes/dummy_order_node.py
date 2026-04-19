from dhquant.core import dhquant_cpp_binding as _cpp
from dhquant.pipeline.base import BaseNode, PipelineContext


class DummyOrderNode(BaseNode):
    def __init__(self, instrument_id: str):
        self.instrument_id = instrument_id
        self.triggered = False

    def process(self, ctx: PipelineContext) -> None:
        if not self.triggered and ctx.current_bar.instrument_id == self.instrument_id:
            order = _cpp.Order()
            order.instrument_id = self.instrument_id
            order.side = _cpp.Side.BUY
            order.offset = _cpp.Offset.OPEN
            order.order_type = _cpp.OrderType.MARKET
            order.quantity = 100
            order.order_id = f"dummy_{ctx.current_time}"

            ctx.submit_order(order)
            self.triggered = True
            print(
                f"[DummyOrderNode] Triggered order at {ctx.current_time} for {self.instrument_id}"
            )
