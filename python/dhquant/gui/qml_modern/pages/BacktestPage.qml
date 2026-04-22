pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"

Item {
    id: root

    // ═══ State ═══
    property int selectedMarketIndex: 1
    property var markets: ["A股", "美股", "港股", "加密货币"]
    property string currentSymbol: "TSLA"
    property string periodValue: "日K"
    property string startDate: "2024-11-04"
    property string endDate: "2026-12-04"
    property int selectedStrategyIndex: 0
    property bool backtestRun: false
    property string backtestMode: "compare" // "combine" or "compare"
    property string capitalValue: "10000"
    property string leverageValue: "1"
    property string feeValue: "0"

    // ═══ Market label helper ═══
    property string marketLabel: markets[selectedMarketIndex]
    property string viewListText: "查看" + marketLabel + "列表"

    // ═══ Selected stocks (multi-select tags) ═══
    ListModel {
        id: selectedStocksModel
        ListElement { code: "TSLA"; name: "Tesla"; market: "美股" }
        ListElement { code: "AAPL"; name: "Apple"; market: "美股" }
        ListElement { code: "002142"; name: "宁波银行"; market: "A股" }
        ListElement { code: "600519"; name: "贵州茅台"; market: "A股" }
    }

    // ═══ Strategy list ═══
    property var strategies: [
        "超买回落-做空策略", "止跌反弹-做多策略", "打出头鸟-做空策略",
        "超卖反弹-做多策略", "均线交叉-做多策略", "MACD背离-做多策略",
        "布林带突破-做多策略", "RSI超卖-做多策略", "双均线-做空策略",
        "KDJ金叉-做多策略", "量价齐升-做多策略"
    ]

    // ═══ Mock stock data per market ═══
    property var marketStocks: ({
        "A股": [
            {code:"000001",name:"平安银行",exchange:"SZSE",board:"MAIN",status:"Normal",industry:"银行",listDate:"1991-04-03"},
            {code:"000002",name:"万科A",exchange:"SZSE",board:"MAIN",status:"Normal",industry:"房地产",listDate:"1991-01-29"},
            {code:"000004",name:"*ST国华",exchange:"SZSE",board:"MAIN",status:"*ST",industry:"信息技术",listDate:"1991-01-14"},
            {code:"000006",name:"深振业A",exchange:"SZSE",board:"MAIN",status:"Normal",industry:"房地产",listDate:"1992-04-27"},
            {code:"000007",name:"全新好",exchange:"SZSE",board:"MAIN",status:"Normal",industry:"传媒",listDate:"1992-04-13"},
            {code:"000008",name:"神州高铁",exchange:"SZSE",board:"MAIN",status:"Normal",industry:"交运设备",listDate:"1992-05-07"},
            {code:"000009",name:"中国宝安",exchange:"SZSE",board:"MAIN",status:"Normal",industry:"新材料",listDate:"1991-06-25"},
            {code:"000010",name:"美丽生态",exchange:"SZSE",board:"MAIN",status:"Normal",industry:"环保",listDate:"1995-10-27"},
            {code:"000011",name:"深物业A",exchange:"SZSE",board:"MAIN",status:"Normal",industry:"房地产",listDate:"1992-03-30"},
            {code:"000012",name:"南玻A",exchange:"SZSE",board:"MAIN",status:"Normal",industry:"建材",listDate:"1992-02-28"},
            {code:"000014",name:"沙河股份",exchange:"SZSE",board:"MAIN",status:"Normal",industry:"房地产",listDate:"1992-06-02"},
            {code:"000016",name:"深康佳A",exchange:"SZSE",board:"MAIN",status:"Normal",industry:"家电",listDate:"1992-03-27"},
            {code:"000017",name:"深中华A",exchange:"SZSE",board:"MAIN",status:"Normal",industry:"自行车",listDate:"1992-03-31"},
            {code:"600519",name:"贵州茅台",exchange:"SSE",board:"MAIN",status:"Normal",industry:"白酒",listDate:"2001-08-27"},
            {code:"002142",name:"宁波银行",exchange:"SZSE",board:"SME",status:"Normal",industry:"银行",listDate:"2007-07-19"},
            {code:"600036",name:"招商银行",exchange:"SSE",board:"MAIN",status:"Normal",industry:"银行",listDate:"2002-04-09"}
        ],
        "美股": [
            {code:"TSLA",name:"Tesla",exchange:"NASDAQ",board:"TECH",status:"Normal",industry:"汽车",listDate:"2010-06-29"},
            {code:"AAPL",name:"Apple",exchange:"NASDAQ",board:"TECH",status:"Normal",industry:"科技",listDate:"1980-12-12"},
            {code:"GOOGL",name:"Alphabet",exchange:"NASDAQ",board:"TECH",status:"Normal",industry:"互联网",listDate:"2004-08-19"},
            {code:"MSFT",name:"Microsoft",exchange:"NASDAQ",board:"TECH",status:"Normal",industry:"软件",listDate:"1986-03-13"},
            {code:"AMZN",name:"Amazon",exchange:"NASDAQ",board:"TECH",status:"Normal",industry:"电商",listDate:"1997-05-15"},
            {code:"NVDA",name:"NVIDIA",exchange:"NASDAQ",board:"TECH",status:"Normal",industry:"芯片",listDate:"1999-01-22"},
            {code:"META",name:"Meta",exchange:"NASDAQ",board:"TECH",status:"Normal",industry:"社交",listDate:"2012-05-18"},
            {code:"JPM",name:"JPMorgan",exchange:"NYSE",board:"FIN",status:"Normal",industry:"银行",listDate:"1969-03-05"}
        ],
        "港股": [
            {code:"00700",name:"腾讯控股",exchange:"HKEX",board:"MAIN",status:"Normal",industry:"互联网",listDate:"2004-06-16"},
            {code:"09988",name:"阿里巴巴",exchange:"HKEX",board:"MAIN",status:"Normal",industry:"电商",listDate:"2019-11-26"},
            {code:"03690",name:"美团",exchange:"HKEX",board:"MAIN",status:"Normal",industry:"本地生活",listDate:"2018-09-20"},
            {code:"01810",name:"小米集团",exchange:"HKEX",board:"MAIN",status:"Normal",industry:"消费电子",listDate:"2018-07-09"},
            {code:"09618",name:"京东集团",exchange:"HKEX",board:"MAIN",status:"Normal",industry:"电商",listDate:"2020-06-18"},
            {code:"00005",name:"汇丰控股",exchange:"HKEX",board:"MAIN",status:"Normal",industry:"银行",listDate:"1972-01-01"}
        ],
        "加密货币": [
            {code:"BTC",name:"Bitcoin",exchange:"Crypto",board:"L1",status:"Normal",industry:"加密货币",listDate:"2009-01-03"},
            {code:"ETH",name:"Ethereum",exchange:"Crypto",board:"L1",status:"Normal",industry:"智能合约",listDate:"2015-07-30"},
            {code:"BNB",name:"BNB",exchange:"Crypto",board:"CEX",status:"Normal",industry:"交易所",listDate:"2017-07-25"},
            {code:"SOL",name:"Solana",exchange:"Crypto",board:"L1",status:"Normal",industry:"高性能链",listDate:"2020-03-16"},
            {code:"XRP",name:"Ripple",exchange:"Crypto",board:"L1",status:"Normal",industry:"跨境支付",listDate:"2012-01-01"},
            {code:"DOGE",name:"Dogecoin",exchange:"Crypto",board:"Meme",status:"Normal",industry:"Meme",listDate:"2013-12-06"}
        ]
    })

    // ═══ Mock backtest results ═══
    property var backtestResults: [
        { name:"超买回落-做空策略", initial:10000, final:10519, returnPct:5.19, trades:1, winRate:100.0, maxDD:0.53, deepDD:0.53, avgHold:2.0, maxHold:2,
          pnl:[4.2], equity:[10000,10050,10120,10200,10350,10400,10519], bench:[10000,10100,10050,10200,10150,10300,10250] },
        { name:"止跌反弹-做多策略", initial:10000, final:12842, returnPct:28.42, trades:6, winRate:83.3, maxDD:8.58, deepDD:2.29, avgHold:6.0, maxHold:10,
          pnl:[5.2,-1.8,8.1,3.4,-2.1,6.8], equity:[10000,10200,10520,10350,10800,11200,11050,11600,12100,12500,12842], bench:[10000,10150,10300,10100,10400,10600,10500,10800,11000,11200,11400] },
        { name:"打出头鸟-做空策略", initial:10000, final:11519, returnPct:15.19, trades:3, winRate:100.0, maxDD:0.0, deepDD:0.0, avgHold:1.0, maxHold:1,
          pnl:[6.2,4.8,3.5], equity:[10000,10200,10620,10800,11100,11300,11519], bench:[10000,10100,10200,10150,10300,10400,10500] },
        { name:"超卖反弹-做多策略", initial:10000, final:11351, returnPct:13.51, trades:2, winRate:100.0, maxDD:0.0, deepDD:0.0, avgHold:1.0, maxHold:1,
          pnl:[8.2,4.9], equity:[10000,10150,10400,10820,11000,11200,11351], bench:[10000,10050,10150,10250,10350,10400,10500] }
    ]

    // ═══ Helper functions ═══
    function addStock(code, name, market) {
        for (var i = 0; i < selectedStocksModel.count; i++) {
            if (selectedStocksModel.get(i).code === code) return;
        }
        selectedStocksModel.append({code: code, name: name, market: market});
        root.currentSymbol = code;
    }

    function removeStock(idx) {
        selectedStocksModel.remove(idx);
    }

    // ═══ Main Layout ═══
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 10

        // ─── Row 1: Query bar ───
        Rectangle {
            Layout.fillWidth: true; Layout.preferredHeight: 56
            radius: 14; color: window.surface; border.color: window.line

            RowLayout {
                anchors.fill: parent; anchors.leftMargin: 16; anchors.rightMargin: 16; spacing: 10

                // Symbol input
                Column { spacing: 2; Layout.preferredWidth: 100
                    Text { text: "股票代码"; color: window.textDim; font.pixelSize: 9 }
                    Rectangle { width: 90; height: 26; radius: 6; color: "#0f151a"; border.color: "#1e2a31"
                        TextInput { anchors.fill: parent; anchors.leftMargin: 8; anchors.rightMargin: 4; verticalAlignment: Text.AlignVCenter; color: window.textPrimary; font.pixelSize: 13; font.family: "Menlo"; text: root.currentSymbol; onTextChanged: root.currentSymbol = text; clip: true }
                    }
                }

                // Market dropdown
                Column { spacing: 2; Layout.preferredWidth: 90
                    Text { text: "市场"; color: window.textDim; font.pixelSize: 9 }
                    Rectangle { width: 80; height: 26; radius: 6; color: "#0f151a"; border.color: "#1e2a31"
                        Text { anchors.centerIn: parent; text: root.marketLabel + " ▼"; color: window.textPrimary; font.pixelSize: 12 }
                        MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor
                            onClicked: { root.selectedMarketIndex = (root.selectedMarketIndex + 1) % 4 }
                        }
                    }
                }

                // Period
                Column { spacing: 2; Layout.preferredWidth: 70
                    Text { text: "周期"; color: window.textDim; font.pixelSize: 9 }
                    Rectangle { width: 60; height: 26; radius: 6; color: "#0f151a"; border.color: "#1e2a31"
                        Text { anchors.centerIn: parent; text: root.periodValue + " ▼"; color: window.textPrimary; font.pixelSize: 12 }
                        MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor
                            onClicked: { var ps=["日K","周K","月K"]; var ci=ps.indexOf(root.periodValue); root.periodValue=ps[(ci+1)%3] }
                        }
                    }
                }

                // Start date
                Column { spacing: 2; Layout.preferredWidth: 120
                    Text { text: "开始日期"; color: window.textDim; font.pixelSize: 9 }
                    Rectangle { width: 110; height: 26; radius: 6; color: "#0f151a"; border.color: "#1e2a31"
                        TextInput { anchors.fill: parent; anchors.leftMargin: 8; verticalAlignment: Text.AlignVCenter; color: window.textPrimary; font.pixelSize: 12; text: root.startDate; onTextChanged: root.startDate=text; clip: true }
                    }
                }

                // End date
                Column { spacing: 2; Layout.preferredWidth: 120
                    Text { text: "结束日期"; color: window.textDim; font.pixelSize: 9 }
                    Rectangle { width: 110; height: 26; radius: 6; color: "#0f151a"; border.color: "#1e2a31"
                        TextInput { anchors.fill: parent; anchors.leftMargin: 8; verticalAlignment: Text.AlignVCenter; color: window.textPrimary; font.pixelSize: 12; text: root.endDate; onTextChanged: root.endDate=text; clip: true }
                    }
                }

                // Query button
                Rectangle { width: 72; height: 32; radius: 8; color: "#2f6ce0"
                    Text { anchors.centerIn: parent; text: "查询"; color: "#f5f7fb"; font.pixelSize: 13; font.bold: true }
                    MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: root.backtestRun = true }
                }

                Item { Layout.fillWidth: true }

                // View list button
                Rectangle { width: 130; height: 32; radius: 8; color: "transparent"; border.color: "#7a4d2a"
                    Text { anchors.centerIn: parent; text: root.viewListText; color: "#d58e53"; font.pixelSize: 11; font.bold: true }
                    MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: stockListPopup.open() }
                }
            }
        }

        // ─── Row 2: Stock tags ───
        Flow {
            Layout.fillWidth: true; spacing: 8
            Repeater {
                model: selectedStocksModel
                delegate: Rectangle {
                    required property int index
                    required property string code
                    required property string name
                    required property string market
                    width: tagRow.implicitWidth + 24; height: 34; radius: 10
                    color: index === 0 ? "#2f6ce0" : "#131a20"
                    border.color: index === 0 ? "#3d7efc" : "#253038"
                    Row {
                        id: tagRow; anchors.centerIn: parent; spacing: 6
                        Text { text: code; color: index === 0 ? "#f4f7fb" : window.textPrimary; font.pixelSize: 12; font.bold: true; font.family: "Menlo" }
                        Text { text: market; color: index === 0 ? "#dce9ff" : window.textDim; font.pixelSize: 10 }
                        Text { text: "×"; color: index === 0 ? "#dce9ff" : "#62707d"; font.pixelSize: 13
                            MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: root.removeStock(index) }
                        }
                    }
                    MouseArea { anchors.fill: parent; z: -1; cursorShape: Qt.PointingHandCursor; onClicked: root.currentSymbol = code }
                }
            }
        }

        // ─── Row 3: Data stats + action bar ───
        RowLayout {
            Layout.fillWidth: true; spacing: 10
            Text { text: "共 501 条数据"; color: window.textMuted; font.pixelSize: 11; font.bold: true }
            Rectangle { width: 80; height: 30; radius: 10; color: "#6e4bc2"
                Text { anchors.centerIn: parent; text: "策略编辑 ▼"; color: "#f4f2fb"; font.pixelSize: 10; font.bold: true }
                MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: strategyPopup.open() }
            }
            Rectangle { width: 90; height: 30; radius: 10; color: "#d86f21"
                Text { anchors.centerIn: parent; text: "策略回测 ▲"; color: "#fff5ea"; font.pixelSize: 10; font.bold: true }
                MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: root.backtestRun = true }
            }
            Text { text: root.backtestRun ? "39 笔交易" : ""; color: "#d7b649"; font.pixelSize: 12; font.bold: true }
            Item { Layout.fillWidth: true }
        }

        // ─── Row 4: Strategy config bar ───
        Rectangle {
            Layout.fillWidth: true; Layout.preferredHeight: 52
            radius: 14; color: window.surface; border.color: window.line

            RowLayout {
                anchors.fill: parent; anchors.leftMargin: 14; anchors.rightMargin: 14; spacing: 8

                // Strategy selector
                Rectangle { width: 200; height: 34; radius: 10; color: "#0f151a"; border.color: "#1e2a31"
                    RowLayout { anchors.fill: parent; anchors.leftMargin: 10; anchors.rightMargin: 8
                        Column { spacing: 1; Layout.fillWidth: true
                            Text { text: "策略("+root.strategies.length+")"; color: window.textDim; font.pixelSize: 8 }
                            Text { text: root.strategies[root.selectedStrategyIndex]; color: window.textPrimary; font.pixelSize: 11; font.bold: true; elide: Text.ElideRight; width: 160 }
                        }
                        Text { text: "▼"; color: window.textDim; font.pixelSize: 10 }
                    }
                    MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: strategyPopup.open() }
                }

                // Initial capital
                Column { spacing: 1
                    Text { text: "初始资金"; color: window.textDim; font.pixelSize: 8 }
                    Rectangle { width: 80; height: 22; radius: 4; color: "#0f151a"; border.color: "#1e2a31"
                        TextInput { anchors.fill: parent; anchors.leftMargin: 6; verticalAlignment: Text.AlignVCenter; color: window.textPrimary; font.pixelSize: 12; font.family: "Menlo"; text: root.capitalValue; onTextChanged: root.capitalValue=text; clip: true }
                    }
                }

                // Leverage
                Column { spacing: 1
                    Text { text: "杠杆"; color: window.textDim; font.pixelSize: 8 }
                    Rectangle { width: 40; height: 22; radius: 4; color: "#0f151a"; border.color: "#1e2a31"
                        TextInput { anchors.fill: parent; anchors.leftMargin: 6; verticalAlignment: Text.AlignVCenter; color: window.textPrimary; font.pixelSize: 12; font.family: "Menlo"; text: root.leverageValue; onTextChanged: root.leverageValue=text; clip: true }
                    }
                }

                // Fee
                Column { spacing: 1
                    Text { text: "手续费%"; color: window.textDim; font.pixelSize: 8 }
                    Rectangle { width: 40; height: 22; radius: 4; color: "#0f151a"; border.color: "#1e2a31"
                        TextInput { anchors.fill: parent; anchors.leftMargin: 6; verticalAlignment: Text.AlignVCenter; color: window.textPrimary; font.pixelSize: 12; font.family: "Menlo"; text: root.feeValue; onTextChanged: root.feeValue=text; clip: true }
                    }
                }

                // Action buttons
                Rectangle { width: 56; height: 32; radius: 8; color: "#2f6ce0"
                    Text { anchors.centerIn: parent; text: "组合"; color: "#eef4ff"; font.pixelSize: 11; font.bold: true }
                    MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: { root.backtestMode="combine"; root.backtestRun=true } }
                }
                Rectangle { width: 56; height: 32; radius: 8; color: "transparent"; border.color: "#2f6ce0"
                    Text { anchors.centerIn: parent; text: "对比"; color: "#6fa1ff"; font.pixelSize: 11; font.bold: true }
                    MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: { root.backtestMode="compare"; root.backtestRun=true } }
                }
                Rectangle { width: 80; height: 32; radius: 8; color: "#4dcf7d"
                    Text { anchors.centerIn: parent; text: "运行回测"; color: "#08120d"; font.pixelSize: 11; font.bold: true }
                    MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: root.backtestRun = true }
                }
                Rectangle { width: 56; height: 32; radius: 8; color: "transparent"; border.color: "#39454f"
                    Text { anchors.centerIn: parent; text: "清除"; color: window.textMuted; font.pixelSize: 11; font.bold: true }
                    MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: root.backtestRun = false }
                }

                Item { Layout.fillWidth: true }
            }
        }

        // ─── Row 5: Results area (scrollable) ───
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            ScrollBar.vertical.policy: ScrollBar.AsNeeded

            ColumnLayout {
                width: parent.width
                spacing: 14

                // Show results only after backtest run
                Repeater {
                    model: root.backtestRun ? root.backtestResults : []

                    delegate: StrategyResultCard {
                        required property var modelData
                        Layout.fillWidth: true
                        strategyName: modelData.name
                        initialCapital: modelData.initial
                        finalCapital: modelData.final
                        returnPct: modelData.returnPct
                        tradeCount: modelData.trades
                        winRate: modelData.winRate
                        maxDrawdown: modelData.maxDD
                        deepDrawdown: modelData.deepDD
                        avgHoldDays: modelData.avgHold
                        maxHoldDays: modelData.maxHold
                        tradesPnl: modelData.pnl
                        equityCurve: modelData.equity
                        benchmarkCurve: modelData.bench
                    }
                }

                // Empty state
                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.backtestRun ? 0 : 200
                    visible: !root.backtestRun

                    Column {
                        anchors.centerIn: parent; spacing: 12
                        Text { text: "📊"; font.pixelSize: 48; anchors.horizontalCenter: parent.horizontalCenter }
                        Text { text: "配置参数后点击「运行回测」查看结果"; color: window.textDim; font.pixelSize: 14; anchors.horizontalCenter: parent.horizontalCenter }
                    }
                }
            }
        }
    }

    // ═══ Popups ═══
    StockListPopup {
        id: stockListPopup
        parent: Overlay.overlay
        marketName: root.marketLabel
        stockData: root.marketStocks[root.marketLabel] || []
        onStockSelected: function(code, name, market) { root.addStock(code, name, market) }
    }

    StrategyPopup {
        id: strategyPopup
        parent: Overlay.overlay
        strategyList: root.strategies
        selectedIndex: root.selectedStrategyIndex
        onStrategySelected: function(idx) { root.selectedStrategyIndex = idx }
        onCreateRequested: { root.strategies = root.strategies.concat(["新策略-" + (root.strategies.length+1)]); root.selectedStrategyIndex = root.strategies.length-1 }
    }
}
