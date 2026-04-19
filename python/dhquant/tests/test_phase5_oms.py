from __future__ import annotations

import sys
from pathlib import Path


def _ensure_native_module_on_path() -> None:
    root = Path(__file__).resolve().parents[3]
    for build_dir in (root / "build" / "cpp", root / "build" / "cmake", root / "build" / "manual2"):
        if build_dir.exists():
            build_str = str(build_dir)
            if build_str not in sys.path:
                sys.path.insert(0, build_str)


def _write_replay_csv(path: Path) -> Path:
    path.write_text(
        "ts_event,instrument_id,open,high,low,close,volume,turnover\n"
        "1713500000000,SSE.600000,10.0,10.5,9.9,10.2,1000,10200.0\n",
        encoding="utf-8",
    )
    return path


def test_order_service_drives_phase5_backtest_flow(tmp_path: Path) -> None:
    _ensure_native_module_on_path()

    from dhquant.core import dhquant_cpp_binding as _cpp
    from dhquant.domain import Offset, OrderIntent, OrderStatus, OrderType, Side
    from dhquant.oms import OrderService

    replay_path = _write_replay_csv(tmp_path / "phase5_replay.csv")

    engine = _cpp.Engine(_cpp.RuntimeMode.BACKTEST)
    engine.load_replay(str(replay_path))
    engine.start()

    service = OrderService(engine)
    order = service.submit(
        OrderIntent(
            instrument_id="SSE.600000",
            side=Side.BUY,
            offset=Offset.OPEN,
            order_type=OrderType.LIMIT,
            quantity=100,
            price=10.5,
        )
    )
    assert order.status is OrderStatus.NEW

    snapshot = service.get_portfolio_snapshot()
    assert snapshot.account_state.frozen_cash > 0.0

    engine.run_backtest(lambda _bar: None)

    final_order = service.get_order(order.order_id)
    assert final_order is not None
    assert final_order.status is OrderStatus.FILLED

    final_snapshot = service.get_portfolio_snapshot()
    assert final_snapshot.account_state.frozen_cash == 0.0
    assert final_snapshot.positions[0].quantity_total == 100


def test_order_service_surfaces_risk_rejections(tmp_path: Path) -> None:
    _ensure_native_module_on_path()

    from dhquant.core import dhquant_cpp_binding as _cpp
    from dhquant.domain import Offset, OrderIntent, OrderStatus, OrderType, RejectReason, Side
    from dhquant.oms import OrderService

    replay_path = _write_replay_csv(tmp_path / "phase5_replay_reject.csv")

    engine = _cpp.Engine(_cpp.RuntimeMode.BACKTEST)
    engine.load_replay(str(replay_path))
    engine.start()

    service = OrderService(engine)
    order = service.submit(
        OrderIntent(
            instrument_id="SSE.999999",
            side=Side.BUY,
            offset=Offset.OPEN,
            order_type=OrderType.LIMIT,
            quantity=100,
            price=10.5,
        )
    )

    assert order.status is OrderStatus.REJECTED
    assert order.reject_reason is RejectReason.INVALID_INSTRUMENT


def test_engine_exposes_phase5_journal_audit_events(tmp_path: Path) -> None:
    _ensure_native_module_on_path()

    from dhquant.core import dhquant_cpp_binding as _cpp

    replay_path = _write_replay_csv(tmp_path / "phase5_replay_journal.csv")

    engine = _cpp.Engine(_cpp.RuntimeMode.BACKTEST)
    engine.load_replay(str(replay_path))
    engine.start()

    intent = _cpp.OrderIntent()
    intent.instrument_id = "SSE.600000"
    intent.side = _cpp.Side.BUY
    intent.offset = _cpp.Offset.OPEN
    intent.order_type = _cpp.OrderType.LIMIT
    intent.quantity = 100
    intent.price = 10.5
    accepted = engine.submit_intent(intent)

    rejected_intent = _cpp.OrderIntent()
    rejected_intent.instrument_id = "SSE.999999"
    rejected_intent.side = _cpp.Side.BUY
    rejected_intent.offset = _cpp.Offset.OPEN
    rejected_intent.order_type = _cpp.OrderType.LIMIT
    rejected_intent.quantity = 100
    rejected_intent.price = 10.5
    rejected = engine.submit_intent(rejected_intent)

    engine.run_backtest(lambda _bar: None)

    saw_risk_accept = False
    saw_risk_reject = False
    saw_trade = False
    saw_portfolio = False
    for index in range(engine.journal_size()):
        event = engine.read_journal(index)
        if event.event_type == _cpp.EventType.RISK:
            risk = event.payload
            if risk.order_id == accepted.order_id and risk.passed:
                saw_risk_accept = True
            if (
                risk.order_id == rejected.order_id
                and not risk.passed
                and risk.reason == _cpp.RejectReason.INVALID_INSTRUMENT
            ):
                saw_risk_reject = True
        elif event.event_type == _cpp.EventType.TRADE:
            trade = event.payload
            if trade.order_id == accepted.order_id and trade.fill_quantity == 100:
                saw_trade = True
        elif event.event_type == _cpp.EventType.PORTFOLIO:
            snapshot = event.payload
            if snapshot.positions and snapshot.positions[0].quantity_total == 100:
                saw_portfolio = True

    assert saw_risk_accept is True
    assert saw_risk_reject is True
    assert saw_trade is True
    assert saw_portfolio is True
