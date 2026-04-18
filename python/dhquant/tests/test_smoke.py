from dhquant import OrderStatus, RuntimeMode, is_terminal, load_config
from dhquant.domain import Instrument, Order


def test_python_domain_defaults() -> None:
    instrument = Instrument(instrument_id="SSE.600000", exchange="SSE", symbol="600000")
    order = Order(
        session_id="paper-20260418",
        order_id="ord-0001",
        instrument_id=instrument.instrument_id,
    )

    assert instrument.currency == "CNY"
    assert order.status is OrderStatus.UNKNOWN
    assert order.instrument_id == "SSE.600000"


def test_terminal_status_helper() -> None:
    assert is_terminal(OrderStatus.FILLED) is True
    assert is_terminal(OrderStatus.CANCELLED) is True
    assert is_terminal(OrderStatus.NEW) is False
    assert RuntimeMode.BACKTEST.value == "backtest"


def test_load_config_merges_base_and_env() -> None:
    config = load_config("dev")

    assert config["app"]["name"] == "dhquant"
    assert config["log"]["level"] == "DEBUG"
    assert config["gateway"]["broker_type"] == "sim"
