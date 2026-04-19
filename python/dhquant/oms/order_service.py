from __future__ import annotations

from dhquant import domain
from dhquant.core import dhquant_cpp_binding as _cpp
from dhquant.runtime import converters


class OrderService:
    def __init__(self, engine: _cpp.Engine) -> None:
        self._engine = engine

    def submit(self, intent: domain.OrderIntent) -> domain.Order:
        if not intent.instrument_id:
            raise ValueError("instrument_id is required")

        raw_intent = converters.order_intent_to_binding(intent)
        return converters.order_from_binding(self._engine.submit_intent(raw_intent))

    def cancel(self, order_id: str) -> domain.Order:
        return converters.order_from_binding(self._engine.cancel_order(order_id))

    def get_order(self, order_id: str) -> domain.Order | None:
        raw_order = self._engine.get_order(order_id)
        if raw_order is None:
            return None
        return converters.order_from_binding(raw_order)

    def get_portfolio_snapshot(self) -> domain.PortfolioSnapshot:
        return converters.portfolio_snapshot_from_binding(self._engine.get_portfolio_snapshot())
