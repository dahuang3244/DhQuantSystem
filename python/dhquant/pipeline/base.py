from abc import ABC, abstractmethod
from dataclasses import dataclass, field
from typing import Any

from dhquant.core import dhquant_cpp_binding as _cpp


@dataclass
class PipelineContext:
    engine: _cpp.Engine
    current_bar: _cpp.Bar | None = None
    current_time: int = 0
    shared: dict[str, Any] = field(default_factory=dict)
    pending_orders: list[_cpp.Order] = field(default_factory=list)

    def submit_order(self, order: _cpp.Order):
        self.pending_orders.append(order)


class BaseNode(ABC):
    @abstractmethod
    def process(self, ctx: PipelineContext) -> None:
        pass
