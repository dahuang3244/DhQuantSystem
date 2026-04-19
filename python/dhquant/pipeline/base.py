from abc import ABC, abstractmethod
from dataclasses import dataclass, field
from typing import Any

from dhquant import domain
from dhquant.core import dhquant_cpp_binding as _cpp
from dhquant.oms.order_service import OrderService


@dataclass
class PipelineContext:
    engine: _cpp.Engine
    order_service: OrderService
    current_bar: _cpp.Bar | None = None
    current_time: int = 0
    shared: dict[str, Any] = field(default_factory=dict)

    def submit_order(self, intent: domain.OrderIntent) -> domain.Order:
        return self.order_service.submit(intent)


class BaseNode(ABC):
    @abstractmethod
    def process(self, ctx: PipelineContext) -> None:
        pass
