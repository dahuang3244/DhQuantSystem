import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"

Item {
    id: root

    function priceColor(change) {
        if (change > 0) return window.positive;
        if (change < 0) return window.negative;
        return window.textMuted;
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 22
        spacing: 18

        GlassPanel {
            Layout.preferredWidth: 380
            Layout.fillHeight: true

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 16

                Column {
                    spacing: 4
                    Text {
                        text: "Market Watch"
                        color: window.textPrimary
                        font.pixelSize: 30
                        font.bold: true
                    }
                    Text {
                        text: "标的列表与主图联动保持不变，结构改成更清晰的交易工作台。"
                        color: window.textMuted
                        font.pixelSize: 12
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 44
                    radius: 16
                    color: window.surfaceSoft
                    border.color: window.line

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 14
                        anchors.rightMargin: 14
                        spacing: 10

                        Rectangle {
                            width: 8
                            height: 8
                            radius: 4
                            color: window.accent
                        }

                        Text {
                            Layout.fillWidth: true
                            text: "A-share live universe"
                            color: window.textMuted
                            font.pixelSize: 11
                        }

                        Text {
                            text: backend.stockList.length + " symbols"
                            color: window.textPrimary
                            font.pixelSize: 11
                            font.bold: true
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 28
                    color: "transparent"

                    RowLayout {
                        anchors.fill: parent
                        spacing: 8

                        Text { text: "代码"; color: window.textDim; font.pixelSize: 10; Layout.preferredWidth: 92 }
                        Text { text: "名称"; color: window.textDim; font.pixelSize: 10; Layout.fillWidth: true }
                        Text { text: "现价"; color: window.textDim; font.pixelSize: 10; Layout.preferredWidth: 54; horizontalAlignment: Text.AlignRight }
                        Text { text: "涨幅"; color: window.textDim; font.pixelSize: 10; Layout.preferredWidth: 62; horizontalAlignment: Text.AlignRight }
                    }
                }

                ListView {
                    id: stockListView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: backend.stockList
                    clip: true
                    spacing: 8
                    boundsBehavior: Flickable.StopAtBounds
                    property int selectedIndex: 0

                    delegate: Rectangle {
                        width: stockListView.width
                        height: 62
                        radius: 18
                        color: index === stockListView.selectedIndex ? "#182127" : (mouseArea.containsMouse ? "#131a20" : "#10161b")
                        border.color: index === stockListView.selectedIndex ? "#2a5a4c" : "#1d272e"

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 14
                            anchors.rightMargin: 14
                            spacing: 8

                            Column {
                                Layout.preferredWidth: 92
                                spacing: 4

                                Text {
                                    text: modelData.code
                                    color: index === stockListView.selectedIndex ? window.textPrimary : window.textMuted
                                    font.pixelSize: 11
                                    font.bold: true
                                    font.family: "Menlo"
                                }
                                Text {
                                    text: modelData.turnover
                                    color: window.textDim
                                    font.pixelSize: 10
                                }
                            }

                            Column {
                                Layout.fillWidth: true
                                spacing: 4

                                Text {
                                    text: modelData.name
                                    color: window.textPrimary
                                    font.pixelSize: 13
                                    font.bold: true
                                    elide: Text.ElideRight
                                }
                                Text {
                                    text: "量 " + modelData.volume + "  额 " + modelData.amount
                                    color: window.textDim
                                    font.pixelSize: 10
                                    elide: Text.ElideRight
                                }
                            }

                            Column {
                                Layout.preferredWidth: 54
                                spacing: 4

                                Text {
                                    width: parent.width
                                    horizontalAlignment: Text.AlignRight
                                    text: Number(modelData.price).toFixed(2)
                                    color: priceColor(modelData.change)
                                    font.pixelSize: 12
                                    font.bold: true
                                    font.family: "Menlo"
                                }
                                Text {
                                    width: parent.width
                                    horizontalAlignment: Text.AlignRight
                                    text: (modelData.change >= 0 ? "+" : "") + Number(modelData.change).toFixed(2)
                                    color: priceColor(modelData.change)
                                    font.pixelSize: 10
                                    font.family: "Menlo"
                                }
                            }

                            Text {
                                Layout.preferredWidth: 62
                                horizontalAlignment: Text.AlignRight
                                text: (modelData.changePct >= 0 ? "+" : "") + Number(modelData.changePct).toFixed(2) + "%"
                                color: priceColor(modelData.changePct)
                                font.pixelSize: 11
                                font.bold: true
                                font.family: "Menlo"
                            }
                        }

                        MouseArea {
                            id: mouseArea
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

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 18

            GlassPanel {
                Layout.fillWidth: true
                Layout.preferredHeight: 136

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 22
                    spacing: 18

                    Column {
                        spacing: 8
                        Text {
                            text: backend.selectedInstrument.instrumentId || ""
                            color: window.textPrimary
                            font.pixelSize: 30
                            font.bold: true
                            font.family: "Menlo"
                        }
                        Row {
                            spacing: 10
                            Text {
                                text: Number(backend.selectedInstrument.lastPrice || 0).toFixed(2)
                                color: priceColor(backend.selectedInstrument.change || 0)
                                font.pixelSize: 26
                                font.bold: true
                                font.family: "Menlo"
                            }
                            Text {
                                text: {
                                    var c = backend.selectedInstrument.change || 0;
                                    return (c >= 0 ? "+" : "") + Number(c).toFixed(2);
                                }
                                color: priceColor(backend.selectedInstrument.change || 0)
                                font.pixelSize: 14
                                font.family: "Menlo"
                            }
                            Text {
                                text: {
                                    var p = backend.selectedInstrument.changePct || 0;
                                    return (p >= 0 ? "+" : "") + Number(p).toFixed(2) + "%";
                                }
                                color: priceColor(backend.selectedInstrument.changePct || 0)
                                font.pixelSize: 14
                                font.family: "Menlo"
                            }
                        }
                    }

                    Item { Layout.fillWidth: true }

                    Repeater {
                        model: [
                            { label: "Open", value: Number(backend.selectedInstrument.open || 0).toFixed(2) },
                            { label: "High", value: Number(backend.selectedInstrument.high || 0).toFixed(2) },
                            { label: "Low", value: Number(backend.selectedInstrument.low || 0).toFixed(2) },
                            { label: "Volume", value: backend.selectedInstrument.volume || "--" },
                            { label: "Turnover", value: backend.selectedInstrument.turnover || "--" }
                        ]

                        Rectangle {
                            Layout.preferredWidth: 108
                            Layout.fillHeight: true
                            radius: 18
                            color: "#11181d"
                            border.color: "#1e2a31"

                            Column {
                                anchors.centerIn: parent
                                spacing: 6
                                Text {
                                    text: modelData.label
                                    color: window.textDim
                                    font.pixelSize: 10
                                    horizontalAlignment: Text.AlignHCenter
                                    width: parent.width
                                }
                                Text {
                                    text: modelData.value
                                    color: window.textPrimary
                                    font.pixelSize: 12
                                    font.bold: true
                                    font.family: "Menlo"
                                    horizontalAlignment: Text.AlignHCenter
                                    width: parent.width
                                }
                            }
                        }
                    }
                }
            }

            GlassPanel {
                Layout.fillWidth: true
                Layout.fillHeight: true

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 12

                    RowLayout {
                        Layout.fillWidth: true

                        Text {
                            text: "Price Structure"
                            color: window.textPrimary
                            font.pixelSize: 18
                            font.bold: true
                        }

                        Text {
                            text: "日线结构保留，重新整理层次和光感。"
                            color: window.textMuted
                            font.pixelSize: 11
                        }

                        Item { Layout.fillWidth: true }

                        Row {
                            spacing: 10

                            Repeater {
                                model: [
                                    { label: "MA5", color: "#d9a557" },
                                    { label: "MA10", color: "#7fbce6" },
                                    { label: "MA20", color: "#5ec3a0" }
                                ]

                                Row {
                                    spacing: 5
                                    Rectangle {
                                        width: 16
                                        height: 2
                                        radius: 1
                                        color: modelData.color
                                        anchors.verticalCenter: parent.verticalCenter
                                    }
                                    Text {
                                        text: modelData.label
                                        color: modelData.color
                                        font.pixelSize: 10
                                    }
                                }
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        property int hoverIndex: -1

                        Canvas {
                            id: klineCanvas
                            anchors.fill: parent
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

                                if (!klData || klData.length === 0)
                                    return;

                                var rightMargin = 68;
                                var bottomMargin = 24;
                                var volAreaRatio = 0.20;
                                var chartW = width - rightMargin;
                                var totalH = height - bottomMargin;
                                var klineH = totalH * (1 - volAreaRatio);
                                var volH = totalH * volAreaRatio;
                                var volTop = klineH + 6;

                                var minP = Infinity;
                                var maxP = -Infinity;
                                var maxVol = 0;
                                for (var i = 0; i < klData.length; i++) {
                                    var d = klData[i];
                                    minP = Math.min(minP, d.low, d.ma5 || d.low, d.ma10 || d.low, d.ma20 || d.low);
                                    maxP = Math.max(maxP, d.high, d.ma5 || d.high, d.ma10 || d.high, d.ma20 || d.high);
                                    maxVol = Math.max(maxVol, d.volume);
                                }

                                var spread = Math.max(0.01, maxP - minP);
                                maxP += spread * 0.08;
                                minP -= spread * 0.08;
                                spread = Math.max(0.01, maxP - minP);

                                var n = klData.length;
                                var xStep = chartW / n;
                                var candleW = Math.max(1, xStep * 0.64);

                                function priceToY(p) { return klineH * (1.0 - (p - minP) / spread); }
                                function volToY(v) { return volTop + volH - (v / maxVol) * (volH - 10); }

                                ctx.strokeStyle = "#22303a";
                                ctx.lineWidth = 0.7;
                                for (var g = 0; g <= 5; g++) {
                                    var gy = klineH * g / 5;
                                    ctx.beginPath();
                                    ctx.moveTo(0, gy);
                                    ctx.lineTo(chartW, gy);
                                    ctx.stroke();
                                    var gp = maxP - spread * g / 5;
                                    ctx.fillStyle = "#66727e";
                                    ctx.font = "10px Menlo";
                                    ctx.fillText(gp.toFixed(2), chartW + 8, gy + 4);
                                }

                                ctx.beginPath();
                                ctx.moveTo(0, volTop);
                                ctx.lineTo(chartW, volTop);
                                ctx.stroke();

                                for (var j = 0; j < n; j++) {
                                    var item = klData[j];
                                    var centerX = j * xStep + xStep / 2;
                                    var rising = item.close >= item.open;
                                    var candleColor = rising ? "#d25b68" : "#32b06f";
                                    var wickTop = priceToY(item.high);
                                    var wickBottom = priceToY(item.low);

                                    ctx.strokeStyle = candleColor;
                                    ctx.lineWidth = 1;
                                    ctx.beginPath();
                                    ctx.moveTo(centerX, wickTop);
                                    ctx.lineTo(centerX, wickBottom);
                                    ctx.stroke();

                                    var openY = priceToY(item.open);
                                    var closeY = priceToY(item.close);
                                    var bodyTop = Math.min(openY, closeY);
                                    var bodyHeight = Math.max(1, Math.abs(closeY - openY));

                                    if (rising) {
                                        ctx.strokeRect(centerX - candleW / 2, bodyTop, candleW, bodyHeight);
                                        ctx.fillStyle = "#11181d";
                                        ctx.fillRect(centerX - candleW / 2 + 1, bodyTop + 1, Math.max(0, candleW - 2), Math.max(0, bodyHeight - 2));
                                    } else {
                                        ctx.fillStyle = candleColor;
                                        ctx.fillRect(centerX - candleW / 2, bodyTop, candleW, bodyHeight);
                                    }

                                    var volumeTop = volToY(item.volume);
                                    ctx.fillStyle = rising ? "rgba(210,91,104,0.46)" : "rgba(50,176,111,0.46)";
                                    ctx.fillRect(centerX - candleW / 2, volumeTop, candleW, volTop + volH - volumeTop);
                                }

                                function drawMA(key, color) {
                                    var started = false;
                                    ctx.strokeStyle = color;
                                    ctx.lineWidth = 1.3;
                                    ctx.beginPath();
                                    for (var p = 0; p < n; p++) {
                                        var value = klData[p][key];
                                        if (!value)
                                            continue;
                                        var px = p * xStep + xStep / 2;
                                        var py = priceToY(value);
                                        if (!started) {
                                            ctx.moveTo(px, py);
                                            started = true;
                                        } else {
                                            ctx.lineTo(px, py);
                                        }
                                    }
                                    ctx.stroke();
                                }

                                drawMA("ma5", "#d9a557");
                                drawMA("ma10", "#7fbce6");
                                drawMA("ma20", "#5ec3a0");

                                ctx.fillStyle = "#66727e";
                                ctx.font = "10px Menlo";
                                ctx.textAlign = "center";
                                var labelStep = Math.max(1, Math.floor(n / 7));
                                for (var dIndex = 0; dIndex < n; dIndex += labelStep) {
                                    var parts = (klData[dIndex].date || "").split("-");
                                    var label = parts.length >= 3 ? parts[1] + "/" + parts[2] : klData[dIndex].date;
                                    ctx.fillText(label, dIndex * xStep + xStep / 2, totalH + 16);
                                }

                                if (hIdx >= 0 && hIdx < n) {
                                    var hoverX = hIdx * xStep + xStep / 2;
                                    ctx.setLineDash([4, 4]);
                                    ctx.strokeStyle = "rgba(255,255,255,0.28)";
                                    ctx.beginPath();
                                    ctx.moveTo(hoverX, 0);
                                    ctx.lineTo(hoverX, totalH);
                                    ctx.stroke();
                                    ctx.setLineDash([]);

                                    var td = klData[hIdx];
                                    var tipW = 152;
                                    var tipH = 106;
                                    var tipX = hoverX + 14;
                                    if (tipX + tipW > chartW)
                                        tipX = hoverX - tipW - 14;
                                    var tipY = 12;

                                    ctx.fillStyle = "rgba(11,16,20,0.94)";
                                    ctx.strokeStyle = "#2f3c45";
                                    ctx.lineWidth = 1;
                                    ctx.beginPath();
                                    var radius = 12;
                                    ctx.moveTo(tipX + radius, tipY);
                                    ctx.lineTo(tipX + tipW - radius, tipY);
                                    ctx.arcTo(tipX + tipW, tipY, tipX + tipW, tipY + radius, radius);
                                    ctx.lineTo(tipX + tipW, tipY + tipH - radius);
                                    ctx.arcTo(tipX + tipW, tipY + tipH, tipX + tipW - radius, tipY + tipH, radius);
                                    ctx.lineTo(tipX + radius, tipY + tipH);
                                    ctx.arcTo(tipX, tipY + tipH, tipX, tipY + tipH - radius, radius);
                                    ctx.lineTo(tipX, tipY + radius);
                                    ctx.arcTo(tipX, tipY, tipX + radius, tipY, radius);
                                    ctx.closePath();
                                    ctx.fill();
                                    ctx.stroke();

                                    var lines = [
                                        ["Date", td.date],
                                        ["Open", td.open.toFixed(2)],
                                        ["High", td.high.toFixed(2)],
                                        ["Low", td.low.toFixed(2)],
                                        ["Close", td.close.toFixed(2)],
                                        ["Vol", (td.volume / 10000).toFixed(0) + "万"]
                                    ];

                                    ctx.textAlign = "left";
                                    ctx.font = "10px Menlo";
                                    for (var l = 0; l < lines.length; l++) {
                                        ctx.fillStyle = "#7f8d98";
                                        ctx.fillText(lines[l][0], tipX + 10, tipY + 18 + l * 15);
                                        ctx.fillStyle = "#eef2f4";
                                        ctx.fillText(lines[l][1], tipX + 56, tipY + 18 + l * 15);
                                    }
                                }
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onPositionChanged: {
                                if (!backend.klineData || backend.klineData.length === 0)
                                    return;
                                var chartW = parent.width - 68;
                                var step = chartW / backend.klineData.length;
                                var idx = Math.floor(mouseX / step);
                                if (idx < 0)
                                    idx = 0;
                                if (idx >= backend.klineData.length)
                                    idx = backend.klineData.length - 1;
                                if (mouseX > chartW)
                                    idx = -1;
                                parent.hoverIndex = idx;
                            }
                            onExited: parent.hoverIndex = -1
                        }
                    }
                }
            }
        }
    }
}
