from __future__ import annotations

import csv
import sys
from pathlib import Path


def _ensure_native_module_on_path() -> None:
    root = Path(__file__).resolve().parents[3]
    for build_dir in (root / "build" / "cpp", root / "build" / "cmake", root / "build" / "manual2"):
        if build_dir.exists():
            build_str = str(build_dir)
            if build_str not in sys.path:
                sys.path.insert(0, build_str)


def test_python_domain_defaults() -> None:
    from dhquant import OrderStatus
    from dhquant.domain import Instrument, Order, OrderIntent

    instrument = Instrument(instrument_id="SSE.600000", exchange="SSE", symbol="600000")
    intent = OrderIntent(instrument_id=instrument.instrument_id)
    order = Order(
        session_id="paper-20260418",
        order_id="ord-0001",
        instrument_id=instrument.instrument_id,
    )

    assert instrument.currency == "CNY"
    assert intent.instrument_id == "SSE.600000"
    assert order.status is OrderStatus.UNKNOWN
    assert order.instrument_id == "SSE.600000"


def test_terminal_status_helper() -> None:
    from dhquant import OrderStatus, RuntimeMode, is_terminal

    assert is_terminal(OrderStatus.FILLED) is True
    assert is_terminal(OrderStatus.CANCELLED) is True
    assert is_terminal(OrderStatus.NEW) is False
    assert RuntimeMode.BACKTEST.value == "backtest"


def test_load_config_merges_base_and_env() -> None:
    from dhquant import load_config

    config = load_config("dev")

    assert config["app"]["name"] == "dhquant"
    assert config["log"]["level"] == "DEBUG"
    assert config["gateway"]["broker_type"] == "sim"


def test_runtime_and_pipeline_import_with_built_native_module() -> None:
    _ensure_native_module_on_path()

    import dhquant.core as core
    from dhquant.pipeline import PipelineExecutor, StrategyPipeline
    from dhquant.runtime.runner import RunnerConfig, StrategyRunner

    assert hasattr(core, "dhquant_cpp_binding")
    assert core.Engine is not None
    assert StrategyRunner is not None
    assert RunnerConfig is not None
    assert StrategyPipeline is not None
    assert PipelineExecutor is not None


def test_mapped_replay_preprocessor_generates_canonical_csv(tmp_path: Path) -> None:
    from dhquant.backtest import MappedBarReplayPreprocessor

    source = tmp_path / "bars.csv"
    source.write_text(
        "symbol,time_s,o,h,l,c,vol,amt,keep\n"
        "600000,1713500000,10.0,10.5,9.9,10.2,1000,10200.0,1\n"
        "600000,1713500060,10.2,10.8,10.1,10.7,1200,12840.0,0\n",
        encoding="utf-8",
    )

    processor = MappedBarReplayPreprocessor(
        {
            "instrument_id": "symbol",
            "ts_event": "time_s",
            "open": "o",
            "high": "h",
            "low": "l",
            "close": "c",
            "volume": "vol",
            "turnover": "amt",
        },
        timestamp_unit="s",
        instrument_transform=lambda symbol: f"SSE.{symbol}",
        row_filter=lambda row: row["keep"] == "1",
    )

    prepared = processor.prepare(source)
    try:
        with prepared.csv_path.open("r", encoding="utf-8", newline="") as handle:
            rows = list(csv.DictReader(handle))
    finally:
        prepared.cleanup()

    assert len(rows) == 1
    assert rows[0]["instrument_id"] == "SSE.600000"
    assert rows[0]["ts_event"] == "1713500000000000000"
    assert rows[0]["close"] == "10.2"


def test_replay_loader_uses_preprocessor_and_cleans_temp_file(tmp_path: Path) -> None:
    from dhquant.backtest import BaseCsvReplayPreprocessor, ReplayDataLoader

    source = tmp_path / "strategy_feed.csv"
    source.write_text(
        "code,ts_ms,open,high,low,close,volume,turnover\n"
        "000001.SZ,1713500000000,10.0,10.5,9.9,10.2,1000,10200.0\n",
        encoding="utf-8",
    )

    class StubEngine:
        def __init__(self) -> None:
            self.loaded_path: str | None = None

        def load_replay(self, csv_path: str) -> None:
            self.loaded_path = csv_path
            with open(csv_path, encoding="utf-8", newline="") as handle:
                rows = list(csv.DictReader(handle))
            assert rows[0]["instrument_id"] == "SZSE.000001.SZ"

    class StrategyReplayPreprocessor(BaseCsvReplayPreprocessor):
        def transform_row(self, raw_row: dict[str, str]) -> dict[str, object]:
            return {
                "ts_event": int(raw_row["ts_ms"]) * 1_000_000,
                "instrument_id": f"SZSE.{raw_row['code']}",
                "open": float(raw_row["open"]),
                "high": float(raw_row["high"]),
                "low": float(raw_row["low"]),
                "close": float(raw_row["close"]),
                "volume": int(raw_row["volume"]),
                "turnover": float(raw_row["turnover"]),
            }

    engine = StubEngine()
    loader = ReplayDataLoader(engine)
    loaded_path = loader.load(source, preprocessor=StrategyReplayPreprocessor())

    assert engine.loaded_path is not None
    assert engine.loaded_path == str(loaded_path)
    assert not loaded_path.exists()
