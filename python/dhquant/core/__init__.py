from __future__ import annotations

import importlib


def _has_runtime_symbols(module: object) -> bool:
    return all(hasattr(module, name) for name in ("Engine", "Order", "Trade", "Bar"))


def _load_binding():
    candidates: list[object] = []

    for module_name in ("dhquant_cpp_binding", "dhquant.core.dhquant_cpp_binding"):
        try:
            module = importlib.import_module(module_name)
        except ImportError:
            continue
        if _has_runtime_symbols(module):
            return module
        candidates.append(module)

    if candidates:
        return candidates[0]
    raise ImportError("Native dhquant binding is unavailable")


try:
    dhquant_cpp_binding = _load_binding()
except ImportError:  # pragma: no cover - native module unavailable
    __all__: list[str] = []
else:
    Engine = dhquant_cpp_binding.Engine
    EngineStatus = dhquant_cpp_binding.EngineStatus
    Order = dhquant_cpp_binding.Order
    OrderStatus = dhquant_cpp_binding.OrderStatus
    PriceLevel = dhquant_cpp_binding.PriceLevel
    RuntimeMode = dhquant_cpp_binding.RuntimeMode
    Trade = dhquant_cpp_binding.Trade

    __all__ = [
        "dhquant_cpp_binding",
        "Engine",
        "EngineStatus",
        "Order",
        "OrderStatus",
        "PriceLevel",
        "RuntimeMode",
        "Trade",
    ]
