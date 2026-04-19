from dhquant.backtest import (
    BaseCsvReplayPreprocessor,
    IdentityReplayPreprocessor,
    MappedBarReplayPreprocessor,
    ReplayDataLoader,
    ReplayPreprocessor,
)
from dhquant.config import load_config
from dhquant.domain import (
    Account,
    Bar,
    Instrument,
    InstrumentType,
    Offset,
    Order,
    OrderIntent,
    OrderStatus,
    OrderType,
    PortfolioSnapshot,
    Position,
    PriceLevel,
    RejectReason,
    RiskEvent,
    RuntimeMode,
    Side,
    Tick,
    Trade,
    is_terminal,
)

__all__ = [
    "Account",
    "Bar",
    "BaseCsvReplayPreprocessor",
    "Engine",
    "IdentityReplayPreprocessor",
    "Instrument",
    "InstrumentType",
    "MappedBarReplayPreprocessor",
    "Offset",
    "OrderIntent",
    "Order",
    "OrderStatus",
    "OrderType",
    "PortfolioSnapshot",
    "Position",
    "PriceLevel",
    "RejectReason",
    "RiskEvent",
    "ReplayDataLoader",
    "ReplayPreprocessor",
    "RuntimeMode",
    "Side",
    "Tick",
    "Trade",
    "is_terminal",
    "load_config",
]


def __getattr__(name: str):
    if name == "Engine":
        from dhquant.core import Engine

        return Engine
    raise AttributeError(name)
