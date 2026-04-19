from __future__ import annotations

import importlib
import importlib.util
import sys
from importlib.machinery import EXTENSION_SUFFIXES
from pathlib import Path


def _has_runtime_symbols(module: object) -> bool:
    required = (
        "Account",
        "Bar",
        "Engine",
        "Order",
        "OrderIntent",
        "PortfolioSnapshot",
        "Position",
        "RejectReason",
        "Trade",
    )
    return all(hasattr(module, name) for name in required)


def _load_binding():
    for module_name in ("dhquant_cpp_binding", "dhquant.core.dhquant_cpp_binding"):
        existing = sys.modules.get(module_name)
        if existing is not None and not _has_runtime_symbols(existing):
            sys.modules.pop(module_name, None)
            if module_name == "dhquant.core.dhquant_cpp_binding":
                globals().pop("dhquant_cpp_binding", None)

    for module_name in ("dhquant_cpp_binding",):
        try:
            module = importlib.import_module(module_name)
        except ImportError:
            continue
        if _has_runtime_symbols(module):
            return module

    local_binding = _load_local_binding_from_build_tree()
    if local_binding is not None and _has_runtime_symbols(local_binding):
        return local_binding

    for module_name in ("dhquant.core.dhquant_cpp_binding",):
        try:
            module = importlib.import_module(module_name)
        except ImportError:
            continue
        if _has_runtime_symbols(module):
            return module
        sys.modules.pop(module_name, None)
        globals().pop("dhquant_cpp_binding", None)

    raise ImportError("Native dhquant binding is unavailable")


def _load_local_binding_from_build_tree():
    root = Path(__file__).resolve().parents[3]
    for build_dir in (root / "build" / "manual2", root / "build" / "cmake", root / "build" / "cpp"):
        if not build_dir.exists():
            continue
        for suffix in EXTENSION_SUFFIXES:
            matches = sorted(build_dir.glob(f"dhquant_cpp_binding*{suffix}"))
            if not matches:
                continue
            sys.modules.pop("dhquant_cpp_binding", None)
            spec = importlib.util.spec_from_file_location("dhquant_cpp_binding", matches[0])
            if spec is None or spec.loader is None:
                continue
            try:
                module = importlib.util.module_from_spec(spec)
                spec.loader.exec_module(module)
            except ImportError:
                continue
            sys.modules["dhquant_cpp_binding"] = module
            return module
    return None


def _export_binding_symbols(module: object) -> None:
    globals().update(
        {
            "Account": module.Account,
            "Engine": module.Engine,
            "EngineStatus": module.EngineStatus,
            "OrderIntent": module.OrderIntent,
            "Order": module.Order,
            "OrderStatus": module.OrderStatus,
            "PortfolioSnapshot": module.PortfolioSnapshot,
            "Position": module.Position,
            "PriceLevel": module.PriceLevel,
            "RejectReason": module.RejectReason,
            "RiskEvent": module.RiskEvent,
            "RuntimeMode": module.RuntimeMode,
            "Trade": module.Trade,
            "dhquant_cpp_binding": module,
        }
    )


try:
    dhquant_cpp_binding = _load_binding()
except ImportError:  # pragma: no cover - native module unavailable
    __all__: list[str] = []
else:
    _export_binding_symbols(dhquant_cpp_binding)

    __all__ = [
        "Account",
        "dhquant_cpp_binding",
        "Engine",
        "EngineStatus",
        "OrderIntent",
        "Order",
        "OrderStatus",
        "PortfolioSnapshot",
        "Position",
        "PriceLevel",
        "RejectReason",
        "RiskEvent",
        "RuntimeMode",
        "Trade",
    ]


def __getattr__(name: str):
    if name in {
        "Account",
        "dhquant_cpp_binding",
        "Engine",
        "EngineStatus",
        "OrderIntent",
        "Order",
        "OrderStatus",
        "PortfolioSnapshot",
        "Position",
        "PriceLevel",
        "RejectReason",
        "RiskEvent",
        "RuntimeMode",
        "Trade",
    }:
        module = _load_binding()
        _export_binding_symbols(module)
        return globals()[name]
    raise AttributeError(name)
