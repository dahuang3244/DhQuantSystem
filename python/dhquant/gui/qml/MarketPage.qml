import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    // ─── Color Tokens (local) ──────────────────────────────────────
    readonly property color colUp: "#EE4D5A"      // Red for up (Chinese convention)
    readonly property color colDown: "#2EC47A"     // Green for down
    readonly property color colFlat: "#97A3BF"
    readonly property color colMa5: "#F4B860"
    readonly property color colMa10: "#58A6FF"
    readonly property color colMa20: "#D86DCD"
    readonly property string colVolUpStr: "rgba(238,77,90,0.55)"
    readonly property string colVolDownStr: "rgba(46,196,122,0.55)"
    readonly property color colGrid: "#1A2340"
    readonly property color colPanelBg: "#111827"
    readonly property color colPanelBorder: "#1E293B"
    readonly property color colHeaderBg: "#0F172A"

    function priceColor(change) {
        if (change > 0) return colUp;
        if (change < 0) return colDown;
        return colFlat;
    }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal
        handle: Rectangle {
            implicitWidth: 6
            color: "transparent"
            // 视觉上的细线
            Rectangle {
                anchors.centerIn: parent
                width: 1; height: parent.height
                color: colPanelBorder
            }
            // 抓取提示条
            Rectangle {
                anchors.centerIn: parent
                width: 4; height: 32
                radius: 2
                color: "#334155"
                opacity: (splitHandleMouse.containsMouse || splitHandleMouse.pressed) ? 1.0 : 0.0
                Behavior on opacity { NumberAnimation { duration: 150 } }
            }
            MouseArea {
                id: splitHandleMouse
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.SizeHorCursor
            }
        }

        // ════════════════════════════════════════════════════════════
        // LEFT PANEL — Stock List Table (Sortable)
        // ════════════════════════════════════════════════════════════
        Rectangle {
            id: leftPanel
            SplitView.preferredWidth: 524
            SplitView.minimumWidth: 524
            SplitView.maximumWidth: 1000
            Layout.fillHeight: true
            color: colPanelBg
            clip: true

            readonly property int contentW: 524

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // ── 表头（弹性布局）──
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 36
                    color: colHeaderBg
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 12
                        anchors.rightMargin: 12
                        spacing: 0
                        Text { text: "代码"; color: window.textMuted; font.pixelSize: 11; Layout.preferredWidth: 80; Layout.fillWidth: true }
                        Text { text: "名称"; color: window.textMuted; font.pixelSize: 11; Layout.preferredWidth: 60; Layout.fillWidth: true }
                        Text { text: "现价"; color: window.textMuted; font.pixelSize: 11; Layout.preferredWidth: 58; Layout.fillWidth: true; horizontalAlignment: Text.AlignRight }
                        Text { text: "涨幅"; color: window.textMuted; font.pixelSize: 11; Layout.preferredWidth: 58; Layout.fillWidth: true; horizontalAlignment: Text.AlignRight }
                        Text { text: "涨跌"; color: window.textMuted; font.pixelSize: 11; Layout.preferredWidth: 52; Layout.fillWidth: true; horizontalAlignment: Text.AlignRight }
                        Text { text: "总量"; color: window.textMuted; font.pixelSize: 11; Layout.preferredWidth: 64; Layout.fillWidth: true; horizontalAlignment: Text.AlignRight }
                        Text { text: "金额"; color: window.textMuted; font.pixelSize: 11; Layout.preferredWidth: 64; Layout.fillWidth: true; horizontalAlignment: Text.AlignRight }
                        Text { text: "换手"; color: window.textMuted; font.pixelSize: 11; Layout.preferredWidth: 64; Layout.fillWidth: true; horizontalAlignment: Text.AlignRight }
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: colPanelBorder }

                // ── 股票列表（弹性布局）──
                ListView {
                    id: stockListView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: backend.stockList
                    boundsBehavior: Flickable.StopAtBounds

                    property int selectedIndex: 0

                    delegate: Rectangle {
                        id: stockDelegate
                        width: stockListView.width
                        height: 34
                        color: {
                            if (index === stockListView.selectedIndex) return "#1E293B";
                            if (stockMouse.containsMouse) return "#151D2E";
                            return "transparent";
                        }

                        Behavior on color { ColorAnimation { duration: 120 } }

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 12
                            anchors.rightMargin: 12
                            spacing: 0

                            Text {
                                text: modelData.code
                                color: index === stockListView.selectedIndex ? "#58A6FF" : window.textPrimary
                                font.pixelSize: 11
                                font.family: "Menlo"
                                Layout.preferredWidth: 80
                                Layout.fillWidth: true
                            }
                            Text {
                                text: modelData.name
                                color: window.textPrimary
                                font.pixelSize: 11
                                Layout.preferredWidth: 60
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                            }
                            Text {
                                text: Number(modelData.price).toFixed(2)
                                color: priceColor(modelData.change)
                                font.pixelSize: 11
                                font.family: "Menlo"
                                Layout.preferredWidth: 58
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignRight
                            }
                            Text {
                                text: (modelData.changePct >= 0 ? "+" : "") + Number(modelData.changePct).toFixed(2) + "%"
                                color: priceColor(modelData.change)
                                font.pixelSize: 11
                                font.family: "Menlo"
                                Layout.preferredWidth: 58
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignRight
                            }
                            Text {
                                text: (modelData.change >= 0 ? "+" : "") + Number(modelData.change).toFixed(2)
                                color: priceColor(modelData.change)
                                font.pixelSize: 11
                                font.family: "Menlo"
                                Layout.preferredWidth: 52
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignRight
                            }
                            Text {
                                text: modelData.volume
                                color: window.textMuted
                                font.pixelSize: 10
                                Layout.preferredWidth: 64
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignRight
                            }
                            Text {
                                text: modelData.amount
                                color: window.textMuted
                                font.pixelSize: 10
                                Layout.preferredWidth: 64
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignRight
                            }
                            Text {
                                text: modelData.turnover
                                color: window.textMuted
                                font.pixelSize: 10
                                Layout.preferredWidth: 64
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignRight
                            }
                        }

                        // Bottom border
                        Rectangle {
                            width: parent.width; height: 1
                            anchors.bottom: parent.bottom
                            color: colPanelBorder
                            opacity: 0.5
                        }

                        MouseArea {
                            id: stockMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                stockListView.selectedIndex = index;
                                backend.selectInstrument(modelData.code);
                            }
                        }
                    }
                }
            }
        }


        // ════════════════════════════════════════════════════════════
        // RIGHT PANEL — K-Line Chart + Volume
        // ════════════════════════════════════════════════════════════
        Rectangle {
            SplitView.fillWidth: true
            Layout.fillHeight: true
            color: colPanelBg

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // ── Top Info Bar ──────────────────────────────────
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 64
                    color: colHeaderBg

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 20
                        anchors.rightMargin: 20
                        spacing: 0

                        // Instrument ID & Name
                        Column {
                            Layout.alignment: Qt.AlignVCenter
                            spacing: 2
                            Row {
                                spacing: 10
                                Text {
                                    text: backend.selectedInstrument.instrumentId || ""
                                    color: window.textPrimary
                                    font.pixelSize: 18
                                    font.bold: true
                                    font.family: "Menlo"
                                }
                                Text {
                                    text: Number(backend.selectedInstrument.lastPrice).toFixed(2)
                                    color: priceColor(backend.selectedInstrument.change || 0)
                                    font.pixelSize: 18
                                    font.bold: true
                                    font.family: "Menlo"
                                    anchors.baseline: parent.children[0].baseline
                                }
                                Text {
                                    text: {
                                        var c = backend.selectedInstrument.change || 0;
                                        return (c >= 0 ? "+" : "") + Number(c).toFixed(2);
                                    }
                                    color: priceColor(backend.selectedInstrument.change || 0)
                                    font.pixelSize: 13
                                    font.family: "Menlo"
                                    anchors.baseline: parent.children[0].baseline
                                }
                                Text {
                                    text: {
                                        var p = backend.selectedInstrument.changePct || 0;
                                        return (p >= 0 ? "+" : "") + Number(p).toFixed(2) + "%";
                                    }
                                    color: priceColor(backend.selectedInstrument.changePct || 0)
                                    font.pixelSize: 13
                                    font.family: "Menlo"
                                    anchors.baseline: parent.children[0].baseline
                                }
                            }
                            Row {
                                spacing: 16
                                Repeater {
                                    model: [
                                        {k: "开", v: backend.selectedInstrument.open || 0},
                                        {k: "高", v: backend.selectedInstrument.high || 0},
                                        {k: "低", v: backend.selectedInstrument.low || 0},
                                        {k: "昨收", v: backend.selectedInstrument.prevClose || 0},
                                    ]
                                    Row {
                                        spacing: 3
                                        Text { text: modelData.k; color: window.textMuted; font.pixelSize: 10 }
                                        Text { text: Number(modelData.v).toFixed(2); color: window.textMuted; font.pixelSize: 10; font.family: "Menlo" }
                                    }
                                }
                                Repeater {
                                    model: [
                                        {k: "量", v: backend.selectedInstrument.volume || ""},
                                        {k: "额", v: backend.selectedInstrument.amount || ""},
                                        {k: "换手", v: backend.selectedInstrument.turnover || ""},
                                    ]
                                    Row {
                                        spacing: 3
                                        Text { text: modelData.k; color: window.textMuted; font.pixelSize: 10 }
                                        Text { text: modelData.v; color: window.textMuted; font.pixelSize: 10; font.family: "Menlo" }
                                    }
                                }
                            }
                        }

                        Item { Layout.fillWidth: true }

                        // MA Legend
                        Row {
                            Layout.alignment: Qt.AlignVCenter
                            spacing: 14
                            Row {
                                spacing: 4
                                Rectangle { width: 14; height: 2; color: colMa5; radius: 1; anchors.verticalCenter: parent.verticalCenter }
                                Text { text: "MA5"; color: colMa5; font.pixelSize: 10 }
                            }
                            Row {
                                spacing: 4
                                Rectangle { width: 14; height: 2; color: colMa10; radius: 1; anchors.verticalCenter: parent.verticalCenter }
                                Text { text: "MA10"; color: colMa10; font.pixelSize: 10 }
                            }
                            Row {
                                spacing: 4
                                Rectangle { width: 14; height: 2; color: colMa20; radius: 1; anchors.verticalCenter: parent.verticalCenter }
                                Text { text: "MA20"; color: colMa20; font.pixelSize: 10 }
                            }
                        }

                        Item { width: 20 }

                        // Period Selector
                        Row {
                            spacing: 4
                            Layout.alignment: Qt.AlignVCenter
                            property int selectedPeriod: 5
                            Repeater {
                                model: [{l:"1分", v:0},{l:"5分", v:1},{l:"15分", v:2},{l:"30分", v:3},{l:"60分", v:4},{l:"日线", v:5}]
                                Rectangle {
                                    width: 42; height: 26; radius: 6
                                    color: modelData.v === 5 ? "#2563EB" : (periodMouse.containsMouse ? "#1E293B" : "transparent")
                                    border.color: modelData.v === 5 ? "#2563EB" : "transparent"
                                    Behavior on color { ColorAnimation { duration: 150 } }
                                    Text {
                                        anchors.centerIn: parent
                                        text: modelData.l
                                        color: modelData.v === 5 ? "#FFFFFF" : window.textMuted
                                        font.pixelSize: 11
                                    }
                                    MouseArea {
                                        id: periodMouse
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        cursorShape: Qt.PointingHandCursor
                                    }
                                }
                            }
                        }
                    }
                }
                Rectangle { Layout.fillWidth: true; height: 1; color: colPanelBorder }

                // ── K-Line Canvas + Volume Canvas ─────────────────
                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    // Crosshair tracking state
                    property int hoverIndex: -1
                    property real hoverX: 0
                    property real hoverY: 0

                    Canvas {
                        id: klineCanvas
                        anchors.fill: parent
                        anchors.bottomMargin: 0

                        property var klData: backend.klineData
                        property int hIdx: parent.hoverIndex

                        onKlDataChanged: requestPaint()
                        onWidthChanged: requestPaint()
                        onHeightChanged: requestPaint()
                        onHIdxChanged: requestPaint()

                        onPaint: {
                            var ctx = getContext("2d");
                            ctx.reset();
                            ctx.clearRect(0, 0, width, height);

                            if (!klData || klData.length === 0) return;

                            var rightMargin = 60;
                            var bottomMargin = 22;
                            var volAreaRatio = 0.22;  // 22% height for volume
                            var chartW = width - rightMargin;
                            var totalH = height - bottomMargin;
                            var klineH = totalH * (1 - volAreaRatio);
                            var volH = totalH * volAreaRatio;
                            var volTop = klineH + 1;
                            var gapBetween = 4;

                            // ── Data bounds ──
                            var minP = Infinity, maxP = -Infinity, maxVol = 0;
                            for (var i = 0; i < klData.length; i++) {
                                var d = klData[i];
                                if (d.low < minP) minP = d.low;
                                if (d.high > maxP) maxP = d.high;
                                // Also consider MA values for bounds
                                if (d.ma5 && d.ma5 < minP) minP = d.ma5;
                                if (d.ma5 && d.ma5 > maxP) maxP = d.ma5;
                                if (d.ma20 && d.ma20 < minP) minP = d.ma20;
                                if (d.ma20 && d.ma20 > maxP) maxP = d.ma20;
                                if (d.volume > maxVol) maxVol = d.volume;
                            }
                            var spread = maxP - minP;
                            maxP += spread * 0.06;
                            minP -= spread * 0.06;
                            spread = maxP - minP;
                            if (spread < 0.01) spread = 1;

                            var n = klData.length;
                            var xStep = chartW / n;
                            var cw = Math.max(1, xStep * 0.65);

                            function priceToY(p) { return klineH * (1.0 - (p - minP) / spread); }
                            function volToY(v) { return volTop + volH - (v / maxVol) * (volH - gapBetween); }

                            // ── Grid lines (horizontal) ──
                            ctx.strokeStyle = colGrid;
                            ctx.lineWidth = 0.5;
                            for (var g = 0; g <= 5; g++) {
                                var gy = klineH * g / 5;
                                ctx.beginPath();
                                ctx.moveTo(0, gy);
                                ctx.lineTo(chartW, gy);
                                ctx.stroke();
                                // Price label
                                var gp = maxP - (spread * g / 5);
                                ctx.fillStyle = "#64748B";
                                ctx.font = "10px Menlo, monospace";
                                ctx.textAlign = "left";
                                ctx.fillText(gp.toFixed(2), chartW + 6, gy + 4);
                            }

                            // Volume area separator
                            ctx.strokeStyle = colGrid;
                            ctx.beginPath();
                            ctx.moveTo(0, volTop);
                            ctx.lineTo(chartW, volTop);
                            ctx.stroke();

                            // Volume label
                            ctx.fillStyle = "#64748B";
                            ctx.font = "9px Menlo, monospace";
                            ctx.textAlign = "left";
                            var volLabel = (maxVol / 100000000).toFixed(2) + "亿";
                            if (maxVol < 100000000) volLabel = (maxVol / 10000).toFixed(0) + "万";
                            ctx.fillText("VOL " + volLabel, 4, volTop + 12);

                            // ── Draw Candles + Volume Bars ──
                            for (var i = 0; i < n; i++) {
                                var d = klData[i];
                                var cx = i * xStep + xStep / 2;
                                var isUp = d.close >= d.open;
                                var candleColor = isUp ? colUp : colDown;

                                // --- Volume bar ---
                                var vTop = volToY(d.volume);
                                var vBot = volTop + volH;
                                ctx.fillStyle = isUp ? colVolUpStr : colVolDownStr;
                                ctx.fillRect(cx - cw / 2, vTop, cw, vBot - vTop);

                                // --- Candle wick (high-low line) ---
                                var topY = priceToY(d.high);
                                var bottomY = priceToY(d.low);
                                ctx.strokeStyle = candleColor;
                                ctx.lineWidth = 1;
                                ctx.beginPath();
                                ctx.moveTo(cx, topY);
                                ctx.lineTo(cx, bottomY);
                                ctx.stroke();

                                // --- Candle body ---
                                var openY = priceToY(d.open);
                                var closeY = priceToY(d.close);
                                var bodyH = Math.abs(closeY - openY);
                                if (bodyH < 1) bodyH = 1;
                                var bodyTop = Math.min(openY, closeY);

                                if (isUp) {
                                    // Hollow candle for up
                                    ctx.strokeStyle = candleColor;
                                    ctx.lineWidth = 1;
                                    ctx.strokeRect(cx - cw / 2, bodyTop, cw, bodyH);
                                    // Fill with bg for true hollow look
                                    ctx.fillStyle = colPanelBg;
                                    ctx.fillRect(cx - cw / 2 + 1, bodyTop + 1, cw - 2, bodyH - 2);
                                } else {
                                    // Filled candle for down
                                    ctx.fillStyle = candleColor;
                                    ctx.fillRect(cx - cw / 2, bodyTop, cw, bodyH);
                                }
                            }

                            // ── Moving Average Lines ──
                            function drawMA(key, color) {
                                ctx.strokeStyle = color;
                                ctx.lineWidth = 1.2;
                                ctx.beginPath();
                                var started = false;
                                for (var m = 0; m < n; m++) {
                                    var val = klData[m][key];
                                    if (!val) continue;
                                    var mx = m * xStep + xStep / 2;
                                    var my = priceToY(val);
                                    if (!started) { ctx.moveTo(mx, my); started = true; }
                                    else ctx.lineTo(mx, my);
                                }
                                ctx.stroke();
                            }
                            drawMA("ma5", colMa5);
                            drawMA("ma10", colMa10);
                            drawMA("ma20", colMa20);

                            // ── Date Labels ──
                            ctx.fillStyle = "#64748B";
                            ctx.font = "9px Menlo, monospace";
                            ctx.textAlign = "center";
                            var labelInterval = Math.max(1, Math.floor(n / 8));
                            for (var di = 0; di < n; di += labelInterval) {
                                var dx = di * xStep + xStep / 2;
                                var dateStr = klData[di].date || "";
                                // Show month-day only
                                var parts = dateStr.split("-");
                                var shortDate = parts.length >= 3 ? parts[1] + "/" + parts[2] : dateStr;
                                ctx.fillText(shortDate, dx, totalH + 14);
                            }

                            // ── Crosshair ──
                            if (hIdx >= 0 && hIdx < n) {
                                var chx = hIdx * xStep + xStep / 2;
                                // Vertical line
                                ctx.strokeStyle = "rgba(255,255,255,0.3)";
                                ctx.lineWidth = 0.5;
                                ctx.setLineDash([4, 3]);
                                ctx.beginPath();
                                ctx.moveTo(chx, 0);
                                ctx.lineTo(chx, totalH);
                                ctx.stroke();
                                ctx.setLineDash([]);

                                // Info tooltip
                                var td = klData[hIdx];
                                var tipW = 140, tipH = 96;
                                var tipX = chx + 12;
                                if (tipX + tipW > chartW) tipX = chx - tipW - 12;
                                var tipY = 10;

                                ctx.fillStyle = "rgba(15,23,42,0.92)";
                                ctx.strokeStyle = "#334155";
                                ctx.lineWidth = 1;
                                // Rounded rect
                                var r = 6;
                                ctx.beginPath();
                                ctx.moveTo(tipX + r, tipY);
                                ctx.lineTo(tipX + tipW - r, tipY);
                                ctx.arcTo(tipX + tipW, tipY, tipX + tipW, tipY + r, r);
                                ctx.lineTo(tipX + tipW, tipY + tipH - r);
                                ctx.arcTo(tipX + tipW, tipY + tipH, tipX + tipW - r, tipY + tipH, r);
                                ctx.lineTo(tipX + r, tipY + tipH);
                                ctx.arcTo(tipX, tipY + tipH, tipX, tipY + tipH - r, r);
                                ctx.lineTo(tipX, tipY + r);
                                ctx.arcTo(tipX, tipY, tipX + r, tipY, r);
                                ctx.closePath();
                                ctx.fill();
                                ctx.stroke();

                                ctx.font = "10px Menlo, monospace";
                                ctx.textAlign = "left";
                                var lines = [
                                    {l: "日期", v: td.date},
                                    {l: "开 ", v: td.open.toFixed(2)},
                                    {l: "高 ", v: td.high.toFixed(2)},
                                    {l: "低 ", v: td.low.toFixed(2)},
                                    {l: "收 ", v: td.close.toFixed(2)},
                                    {l: "量 ", v: (td.volume / 10000).toFixed(0) + "万"},
                                ];
                                for (var li = 0; li < lines.length; li++) {
                                    ctx.fillStyle = "#94A3B8";
                                    ctx.fillText(lines[li].l, tipX + 8, tipY + 16 + li * 13);
                                    ctx.fillStyle = "#F1F5F9";
                                    ctx.fillText(lines[li].v, tipX + 40, tipY + 16 + li * 13);
                                }
                            }
                        }
                    }

                    // Mouse tracker for crosshair
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onPositionChanged: {
                            if (!backend.klineData || backend.klineData.length === 0) return;
                            var rightMargin = 60;
                            var chartW = parent.width - rightMargin;
                            var n = backend.klineData.length;
                            var xStep = chartW / n;
                            var idx = Math.floor(mouseX / xStep);
                            if (idx < 0) idx = 0;
                            if (idx >= n) idx = n - 1;
                            if (mouseX > chartW) idx = -1;
                            parent.hoverIndex = idx;
                            parent.hoverX = mouseX;
                            parent.hoverY = mouseY;
                        }
                        onExited: {
                            parent.hoverIndex = -1;
                        }
                    }
                }
            }
        }
    }
}
