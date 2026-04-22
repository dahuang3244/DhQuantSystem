from __future__ import annotations

import math
import random
from datetime import datetime, timedelta

from PySide6.QtCore import Property, QObject, QTimer, Signal, Slot


def _now_text() -> str:
    return datetime.now().strftime("%H:%M:%S")


class TradingDemoBackend(QObject):
    accountChanged = Signal()  # noqa: N815
    selectedInstrumentChanged = Signal()  # noqa: N815
    orderBookChanged = Signal()  # noqa: N815
    orderListChanged = Signal()  # noqa: N815
    positionListChanged = Signal()  # noqa: N815
    riskRulesChanged = Signal()  # noqa: N815
    activityFeedChanged = Signal()  # noqa: N815
    kpiCardsChanged = Signal()  # noqa: N815
    klineDataChanged = Signal()  # noqa: N815
    stockListChanged = Signal()  # noqa: N815

    def __init__(self) -> None:
        super().__init__()
        self._timer = QTimer(self)
        self._timer.setInterval(1800)
        self._timer.timeout.connect(self._advance_demo_state)
        self._load_demo_state()
        self._timer.start()

    def _load_demo_state(self) -> None:
        self._instrument_id = "600000.SH"
        self._last_price = 12.48
        self._prev_close = 12.12
        self._order_sequence = 1003
        self._trade_sequence = 3001
        self._cash = 1_250_000.0
        self._frozen_cash = 25_000.0
        self._equity = 1_286_420.0
        self._orders = [
            {
                "orderId": "ord-1001",
                "instrumentId": "600000.SH",
                "side": "Buy",
                "quantity": 2000,
                "filledQuantity": 1200,
                "price": 12.46,
                "status": "Partially Filled",
                "updatedAt": "09:31:12",
            },
            {
                "orderId": "ord-1002",
                "instrumentId": "000001.SZ",
                "side": "Sell",
                "quantity": 1000,
                "filledQuantity": 1000,
                "price": 10.82,
                "status": "Filled",
                "updatedAt": "09:33:47",
            },
        ]
        self._positions = [
            {
                "instrumentId": "600000.SH",
                "quantityTotal": 5200,
                "quantityAvailable": 4000,
                "averagePrice": 12.11,
                "marketValue": 64896.0,
                "pnl": 1924.0,
            },
            {
                "instrumentId": "000001.SZ",
                "quantityTotal": 3000,
                "quantityAvailable": 3000,
                "averagePrice": 10.36,
                "marketValue": 32460.0,
                "pnl": 1380.0,
            },
        ]
        self._risk_rules = [
            {
                "name": "Single Order Limit",
                "status": "Healthy",
                "detail": "<= 50,000 shares",
            },
            {
                "name": "Price Deviation",
                "status": "Watch",
                "detail": "<= 2.00% from last price",
            },
            {"name": "Buying Power", "status": "Healthy", "detail": "cash available"},
            {
                "name": "Position Exposure",
                "status": "Healthy",
                "detail": "<= 20.00% per symbol",
            },
        ]
        self._activity_feed = [
            {
                "time": "09:30:01",
                "category": "Engine",
                "message": "Paper trading workspace initialized",
            },
            {
                "time": "09:31:12",
                "category": "OMS",
                "message": "ord-1001 partially filled 1200 @ 12.46",
            },
            {"time": "09:33:47", "category": "OMS", "message": "ord-1002 filled 1000 @ 10.82"},
        ]
        self._order_book = []
        self._refresh_order_book()
        self._kline_data = []
        self._generate_kline_data()
        self._stock_list = []
        self._generate_stock_list()

    # ─── Stock List (rich mock data resembling A-share market) ──────────
    def _generate_stock_list(self) -> None:
        """Generate a realistic A-share stock list with comprehensive fields."""
        stock_defs = [
            ("600000.SH", "浦发银行", 13.63, 13.95, 128.28),
            ("600004.SH", "白云机场", 10.01, 9.94, 35.27),
            ("600006.SH", "东风股份", 7.55, 7.61, 52.16),
            ("600007.SH", "中国国贸", 20.55, 20.47, 3.15),
            ("600008.SH", "首创环保", 3.13, 3.15, 90.59),
            ("600009.SH", "上海机场", 32.32, 31.99, 22.94),
            ("600010.SH", "包钢股份", 2.79, 2.80, 1206.0),
            ("600011.SH", "华能国际", 7.38, 7.33, 101.3),
            ("600012.SH", "皖通高速", 14.59, 14.16, 10.35),
            ("600015.SH", "华夏银行", 7.57, 7.72, 194.43),
            ("600016.SH", "民生银行", 4.60, 4.69, 477.72),
            ("600017.SH", "日照港", 3.21, 3.17, 56.82),
            ("600018.SH", "上港集团", 5.64, 5.75, 108.56),
            ("600019.SH", "宝钢股份", 6.93, 7.02, 257.20),
            ("600020.SH", "中原高速", 4.45, 4.45, 17.56),
            ("600021.SH", "上海电力", 15.31, 14.71, 192.25),
            ("600022.SH", "山东钢铁", 1.51, 1.52, 75.15),
            ("600023.SH", "浙能电力", 5.08, 5.14, 216.20),
            ("600025.SH", "华能水电", 9.12, 9.15, 47.09),
            ("600026.SH", "中远海能", 10.35, 10.55, 50.78),
            ("600027.SH", "华电国际", 5.30, 5.33, 180.08),
            ("600028.SH", "中国石化", 5.71, 5.76, 246.67),
            ("600029.SH", "南方航空", 6.04, 5.97, 80.65),
            ("600030.SH", "中信证券", 31.48, 31.54, 248.69),
            ("600031.SH", "三一重工", 21.03, 21.23, 99.38),
        ]
        self._stock_list = []
        for code, name, price, prev_close, total_vol_wan in stock_defs:
            change = price - prev_close
            change_pct = change / prev_close * 100.0
            # random realistic values for amount, turnover, PE, amplitude
            amount_yi = total_vol_wan * price / 10000.0
            turnover = random.uniform(0.3, 5.5)
            pe = random.uniform(4.0, 35.0)
            amplitude = abs(change_pct) + random.uniform(0.2, 2.5)
            self._stock_list.append(
                {
                    "code": code,
                    "name": name,
                    "price": round(price, 2),
                    "prevClose": round(prev_close, 2),
                    "change": round(change, 2),
                    "changePct": round(change_pct, 2),
                    "volume": f"{total_vol_wan:.0f}万",
                    "amount": f"{amount_yi:.2f}亿",
                    "turnover": f"{turnover:.2f}%",
                    "pe": round(pe, 2),
                    "amplitude": f"{amplitude:.2f}%",
                    "high": round(price + random.uniform(0, 0.3), 2),
                    "low": round(price - random.uniform(0, 0.3), 2),
                    "open": round(prev_close + random.uniform(-0.15, 0.15), 2),
                }
            )

    # ─── K-Line Data with volume ──────────────────────────────────────
    def _generate_kline_data(self) -> None:
        """Generate realistic candlestick data with OHLCV + date + MA values."""
        self._kline_data = []
        base_price = self._last_price * 0.85  # start a bit lower for a nicer chart shape
        base_date = datetime(2026, 1, 5)
        prices_close = []

        for i in range(120):
            # Realistic random walk with trend
            trend = math.sin(i * 0.04) * 0.15
            momentum = math.cos(i * 0.08) * 0.08
            noise = random.gauss(0, 0.12)
            change = trend + momentum + noise

            open_p = base_price
            close_p = open_p + change
            high_p = max(open_p, close_p) + abs(random.gauss(0, 0.15))
            low_p = min(open_p, close_p) - abs(random.gauss(0, 0.15))
            low_p = max(low_p, 0.5)

            # Volume: higher on big moves, add random base
            vol_base = random.randint(30000, 180000) * 100
            vol_spike = int(abs(change) * 800000)
            volume = vol_base + vol_spike

            # Skip weekends
            day = base_date + timedelta(days=i + (i // 5) * 2)
            date_str = day.strftime("%Y-%m-%d")

            prices_close.append(round(close_p, 2))

            # Calculate moving averages
            ma5 = (
                sum(prices_close[-5:]) / min(5, len(prices_close))
                if len(prices_close) >= 1
                else close_p
            )
            ma10 = (
                sum(prices_close[-10:]) / min(10, len(prices_close))
                if len(prices_close) >= 1
                else close_p
            )
            ma20 = (
                sum(prices_close[-20:]) / min(20, len(prices_close))
                if len(prices_close) >= 1
                else close_p
            )

            self._kline_data.append(
                {
                    "open": round(open_p, 2),
                    "high": round(high_p, 2),
                    "low": round(low_p, 2),
                    "close": round(close_p, 2),
                    "volume": volume,
                    "date": date_str,
                    "ma5": round(ma5, 2),
                    "ma10": round(ma10, 2),
                    "ma20": round(ma20, 2),
                }
            )
            base_price = close_p

    @Property("QVariantMap", notify=accountChanged)
    def account(self) -> dict:
        available = self._cash - self._frozen_cash
        return {
            "accountId": "paper-sim-001",
            "cash": round(self._cash, 2),
            "frozenCash": round(self._frozen_cash, 2),
            "availableCash": round(available, 2),
            "equity": round(self._equity, 2),
            "unrealizedPnl": round(self._equity - 1_250_000.0, 2),
        }

    @Property("QVariantMap", notify=selectedInstrumentChanged)
    def selectedInstrument(self) -> dict:  # noqa: N802
        change = self._last_price - self._prev_close
        change_pct = change / self._prev_close * 100.0 if self._prev_close else 0
        return {
            "instrumentId": self._instrument_id,
            "lastPrice": round(self._last_price, 2),
            "prevClose": round(self._prev_close, 2),
            "change": round(change, 2),
            "changePct": round(change_pct, 2),
            "high": round(self._last_price + random.uniform(0.05, 0.35), 2),
            "low": round(self._last_price - random.uniform(0.05, 0.35), 2),
            "open": round(self._prev_close + random.uniform(-0.1, 0.2), 2),
            "volume": "128.28万",
            "amount": "17.69亿",
            "turnover": "1.79%",
            "session": "09:30-15:00",
            "mode": "Prototype",
        }

    @Property("QVariantList", notify=orderBookChanged)
    def orderBook(self) -> list[dict]:  # noqa: N802
        return self._order_book

    @Property("QVariantList", notify=orderListChanged)
    def orderList(self) -> list[dict]:  # noqa: N802
        return self._orders

    @Property("QVariantList", notify=positionListChanged)
    def positionList(self) -> list[dict]:  # noqa: N802
        return self._positions

    @Property("QVariantList", notify=riskRulesChanged)
    def riskRules(self) -> list[dict]:  # noqa: N802
        return self._risk_rules

    @Property("QVariantList", notify=activityFeedChanged)
    def activityFeed(self) -> list[dict]:  # noqa: N802
        return self._activity_feed

    @Property("QVariantList", notify=kpiCardsChanged)
    def kpiCards(self) -> list[dict]:  # noqa: N802
        filled_count = sum(1 for order in self._orders if order["status"] == "Filled")
        live_count = sum(
            1
            for order in self._orders
            if order["status"] not in {"Filled", "Cancelled", "Rejected"}
        )
        return [
            {"label": "Engine State", "value": "Prototype", "tone": "neutral"},
            {"label": "Live Orders", "value": str(live_count), "tone": "accent"},
            {"label": "Filled Today", "value": str(filled_count), "tone": "good"},
            {"label": "Risk Flags", "value": self._risk_flag_count(), "tone": "warn"},
        ]

    @Property("QVariantList", notify=klineDataChanged)
    def klineData(self) -> list[dict]:  # noqa: N802
        return self._kline_data

    @Property("QVariantList", notify=stockListChanged)
    def stockList(self) -> list[dict]:  # noqa: N802
        return self._stock_list

    @Slot(str)
    def selectInstrument(self, instrument_id: str) -> None:  # noqa: N802
        cleaned = instrument_id.strip()
        if not cleaned:
            return
        self._instrument_id = cleaned
        # Find stock in list for prev_close
        for s in self._stock_list:
            if s["code"] == cleaned:
                self._last_price = s["price"]
                self._prev_close = s["prevClose"]
                break
        else:
            self._last_price = round(random.uniform(8.0, 28.0), 2)
            self._prev_close = round(self._last_price - random.uniform(-0.5, 0.5), 2)
        self._refresh_order_book()
        self._append_log("Market", f"instrument switched to {cleaned}")
        self._generate_kline_data()
        self.selectedInstrumentChanged.emit()
        self.orderBookChanged.emit()
        self.klineDataChanged.emit()

    @Slot(str, str, int, float)
    def placeOrder(  # noqa: N802
        self, side: str, instrument_id: str, quantity: int, price: float
    ) -> None:
        cleaned = instrument_id.strip() or self._instrument_id
        if quantity <= 0 or price <= 0:
            self._append_log("Risk", "order rejected in prototype: quantity and price must be > 0")
            return

        self._order_sequence += 1
        order = {
            "orderId": f"ord-{self._order_sequence}",
            "instrumentId": cleaned,
            "side": side.title(),
            "quantity": quantity,
            "filledQuantity": 0,
            "price": round(price, 2),
            "status": "Pending New",
            "updatedAt": _now_text(),
        }
        self._orders.insert(0, order)
        self._frozen_cash += quantity * price * 0.12
        self._append_log(
            "OMS", f"{order['orderId']} accepted for {cleaned} {side} {quantity} @ {price:.2f}"
        )
        self.orderListChanged.emit()
        self.accountChanged.emit()
        self.kpiCardsChanged.emit()

    @Slot(str)
    def cancelOrder(self, order_id: str) -> None:  # noqa: N802
        for order in self._orders:
            if order["orderId"] != order_id:
                continue
            if order["status"] in {"Filled", "Cancelled", "Rejected"}:
                self._append_log(
                    "OMS", f"{order_id} cannot be cancelled in state {order['status']}"
                )
                return
            order["status"] = "Cancelled"
            order["updatedAt"] = _now_text()
            self._frozen_cash = max(
                0.0, self._frozen_cash - order["quantity"] * order["price"] * 0.10
            )
            self._append_log("OMS", f"{order_id} cancelled by trader")
            self.orderListChanged.emit()
            self.accountChanged.emit()
            self.kpiCardsChanged.emit()
            return

    @Slot()
    def resetDemoData(self) -> None:  # noqa: N802
        self._load_demo_state()
        self.accountChanged.emit()
        self.selectedInstrumentChanged.emit()
        self.orderBookChanged.emit()
        self.orderListChanged.emit()
        self.positionListChanged.emit()
        self.riskRulesChanged.emit()
        self.activityFeedChanged.emit()
        self.kpiCardsChanged.emit()
        self.stockListChanged.emit()

    def _advance_demo_state(self) -> None:
        self._last_price = round(
            max(1.0, self._last_price + random.uniform(-0.08, 0.08)),
            2,
        )
        self._equity = round(self._equity + random.uniform(-680.0, 920.0), 2)
        self._refresh_order_book()
        self._update_positions()
        self._fill_pending_order()
        self._roll_risk_state()
        # Update stock list prices slightly
        self._tick_stock_list()
        self.selectedInstrumentChanged.emit()
        self.orderBookChanged.emit()
        self.positionListChanged.emit()
        self.accountChanged.emit()
        self.riskRulesChanged.emit()
        self.kpiCardsChanged.emit()
        self.stockListChanged.emit()

    def _tick_stock_list(self) -> None:
        """Simulate tiny price movements in the stock list."""
        for s in self._stock_list:
            delta = random.gauss(0, 0.03)
            s["price"] = round(max(0.5, s["price"] + delta), 2)
            s["change"] = round(s["price"] - s["prevClose"], 2)
            s["changePct"] = round(s["change"] / s["prevClose"] * 100, 2) if s["prevClose"] else 0

    def _fill_pending_order(self) -> None:
        for order in self._orders:
            if order["status"] not in {"Pending New", "Partially Filled"}:
                continue
            remaining = order["quantity"] - order["filledQuantity"]
            if remaining <= 0:
                continue
            step = min(remaining, max(100, remaining // 2))
            order["filledQuantity"] += step
            order["updatedAt"] = _now_text()
            if order["filledQuantity"] >= order["quantity"]:
                order["status"] = "Filled"
                self._trade_sequence += 1
                self._append_log(
                    "Trade",
                    (
                        f"trd-{self._trade_sequence} matched "
                        f"{order['orderId']} {order['quantity']} @ {order['price']:.2f}"
                    ),
                )
            else:
                order["status"] = "Partially Filled"
            self._frozen_cash = max(0.0, self._frozen_cash - step * order["price"] * 0.10)
            self.orderListChanged.emit()
            return

    def _update_positions(self) -> None:
        for position in self._positions:
            mark = (
                self._last_price
                if position["instrumentId"] == self._instrument_id
                else random.uniform(9, 18)
            )
            position["marketValue"] = round(position["quantityTotal"] * mark, 2)
            position["pnl"] = round(
                position["quantityTotal"] * (mark - position["averagePrice"]),
                2,
            )

    def _roll_risk_state(self) -> None:
        deviation = abs(self.selectedInstrument["changePct"])
        self._risk_rules[1]["status"] = "Watch" if deviation < 3.5 else "Alert"
        self._risk_rules[1]["detail"] = f"{deviation:.2f}% from open"
        exposure = self._positions[0]["marketValue"] / max(self._equity, 1.0) * 100.0
        self._risk_rules[3]["status"] = "Alert" if exposure > 20.0 else "Healthy"
        self._risk_rules[3]["detail"] = f"{exposure:.2f}% on top symbol"

    def _refresh_order_book(self) -> None:
        self._order_book = []
        base = self._last_price
        for level in range(5):
            offset = (5 - level) * 0.01
            self._order_book.append(
                {
                    "bidPrice": round(base - offset, 2),
                    "bidQty": random.randrange(300, 2800, 100),
                    "askPrice": round(base + 0.01 + level * 0.01, 2),
                    "askQty": random.randrange(300, 2800, 100),
                    "level": f"L{5 - level}",
                }
            )

    def _append_log(self, category: str, message: str) -> None:
        self._activity_feed.insert(
            0, {"time": _now_text(), "category": category, "message": message}
        )
        self._activity_feed = self._activity_feed[:18]
        self.activityFeedChanged.emit()

    def _risk_flag_count(self) -> str:
        count = sum(1 for item in self._risk_rules if item["status"] in {"Watch", "Alert"})
        return str(count)
