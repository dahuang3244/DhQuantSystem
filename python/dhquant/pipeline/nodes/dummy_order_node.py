from dhquant import domain
from dhquant.pipeline.base import BaseNode, PipelineContext


class DummyOrderNode(BaseNode):
    def __init__(self, instrument_id: str):
        self.instrument_id = instrument_id
        self.triggered = False

    def process(self, ctx: PipelineContext) -> None:
        if not self.triggered and ctx.current_bar.instrument_id == self.instrument_id:
            intent = domain.OrderIntent(
                instrument_id=self.instrument_id,
                side=domain.Side.BUY,
                offset=domain.Offset.OPEN,
                order_type=domain.OrderType.MARKET,
                quantity=100,
            )

            ctx.submit_order(intent)
            self.triggered = True
            print(
                f"[DummyOrderNode] Triggered order at {ctx.current_time} for {self.instrument_id}"
            )
