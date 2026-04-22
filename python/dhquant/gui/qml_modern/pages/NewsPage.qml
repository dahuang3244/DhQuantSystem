import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"

Item {
    id: root

    property var insights: [
        "指数情绪偏中性，资金追逐集中在半导体与高股息板块。",
        "盘面缺少一致性主线，短线交易更依赖节奏与量能确认。",
        "若午后放量站稳，风险偏好可能从防守转向进攻。"
    ]

    property var newsFeed: [
        { tag: "Macro", title: "美元与大宗商品同步回落，风险资产波动率下降", summary: "外围市场波动收缩，给 A 股风险偏好留下修复窗口。", time: "08:45" },
        { tag: "Semis", title: "晶圆厂扩产计划落地，设备与材料链再获订单确认", summary: "设备、材料与封测环节出现同步扩散，产业链景气度延续。", time: "09:18" },
        { tag: "AI", title: "算力基础设施需求延续，液冷与电源环节关注度抬升", summary: "订单端增量明确，但高位标的分化开始增大。", time: "10:06" },
        { tag: "Rates", title: "利率端平稳，红利策略仍具相对收益防守价值", summary: "银行、公用事业与运营商板块继续提供波动缓冲。", time: "11:22" },
        { tag: "Flow", title: "北向风格偏均衡，题材与权重之间切换速度加快", summary: "市场尚未形成单边主升结构，择时仍然关键。", time: "13:37" },
        { tag: "Policy", title: "新一轮产业政策讨论升温，关注设备国产替代节奏", summary: "政策预期正在改善，但兑现时点仍需持续跟踪。", time: "14:16" }
    ]

    RowLayout {
        anchors.fill: parent
        anchors.margins: 22
        spacing: 18

        ColumnLayout {
            Layout.preferredWidth: 400
            Layout.fillHeight: true
            spacing: 18

            GlassPanel {
                Layout.fillWidth: true
                Layout.preferredHeight: 150

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 10

                    Text {
                        text: "News Intelligence"
                        color: window.textPrimary
                        font.pixelSize: 30
                        font.bold: true
                    }
                    Text {
                        text: "保留搜索、情绪、资讯列表，但改成研究终端的节奏。"
                        color: window.textMuted
                        font.pixelSize: 12
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
                            anchors.rightMargin: 10
                            spacing: 8

                            Text {
                                text: "Search"
                                color: window.textDim
                                font.pixelSize: 11
                            }

                            TextField {
                                Layout.fillWidth: true
                                placeholderText: "搜索股票代码、行业主题、资讯关键词"
                                color: window.textPrimary
                                placeholderTextColor: window.textDim
                                selectedTextColor: "#0b0f12"
                                selectionColor: window.accent
                                background: Item {}
                            }

                            Rectangle {
                                Layout.preferredWidth: 80
                                Layout.preferredHeight: 30
                                radius: 15
                                color: window.accent

                                Text {
                                    anchors.centerIn: parent
                                    text: "Analyze"
                                    color: "#08100d"
                                    font.pixelSize: 11
                                    font.bold: true
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
                    spacing: 16

                    Text {
                        text: "Sentiment Gauge"
                        color: window.textPrimary
                        font.pixelSize: 18
                        font.bold: true
                    }

                    Item {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: 240
                        Layout.preferredHeight: 240

                        Canvas {
                            anchors.fill: parent
                            onPaint: {
                                var ctx = getContext("2d");
                                ctx.reset();
                                var cx = width / 2;
                                var cy = height / 2;
                                var radius = 82;

                                ctx.lineWidth = 16;
                                ctx.strokeStyle = "#1d262d";
                                ctx.beginPath();
                                ctx.arc(cx, cy, radius, Math.PI * 0.8, Math.PI * 2.2, false);
                                ctx.stroke();

                                ctx.strokeStyle = window.accent;
                                ctx.beginPath();
                                ctx.arc(cx, cy, radius, Math.PI * 0.8, Math.PI * 1.62, false);
                                ctx.stroke();
                            }
                        }

                        Column {
                            anchors.centerIn: parent
                            spacing: 4
                            Text {
                                text: "48"
                                color: window.textPrimary
                                font.pixelSize: 64
                                font.bold: true
                            }
                            Text {
                                text: "Neutral to constructive"
                                color: window.textMuted
                                font.pixelSize: 12
                                horizontalAlignment: Text.AlignHCenter
                                width: parent.width
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 1
                        color: window.line
                    }

                    Repeater {
                        model: insights
                        delegate: RowLayout {
                            Layout.fillWidth: true
                            spacing: 10

                            Rectangle {
                                Layout.preferredWidth: 8
                                Layout.preferredHeight: 8
                                radius: 4
                                color: index === 0 ? window.accent : "#3c4a55"
                            }

                            Text {
                                Layout.fillWidth: true
                                text: modelData
                                color: window.textMuted
                                font.pixelSize: 12
                                wrapMode: Text.Wrap
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
                spacing: 14

                RowLayout {
                    Layout.fillWidth: true

                    Column {
                        spacing: 4
                        Text {
                            text: "Live Feed"
                            color: window.textPrimary
                            font.pixelSize: 24
                            font.bold: true
                        }
                        Text {
                            text: "按研究阅读方式排版，而不是普通资讯流卡片堆叠。"
                            color: window.textMuted
                            font.pixelSize: 11
                        }
                    }

                    Item { Layout.fillWidth: true }

                    Rectangle {
                        Layout.preferredWidth: 128
                        Layout.preferredHeight: 32
                        radius: 16
                        color: "#11181d"
                        border.color: "#243038"

                        Row {
                            anchors.centerIn: parent
                            spacing: 8
                            Rectangle {
                                width: 7
                                height: 7
                                radius: 3.5
                                color: window.accent
                            }
                            Text {
                                text: "AI summary on"
                                color: window.textMuted
                                font.pixelSize: 11
                            }
                        }
                    }
                }

                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    spacing: 10
                    model: newsFeed

                    delegate: Rectangle {
                        width: ListView.view ? ListView.view.width : 0
                        height: 112
                        radius: 22
                        color: index % 2 === 0 ? "#11181d" : "#0f151a"
                        border.color: "#1f2a32"

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 18
                            spacing: 16

                            Rectangle {
                                Layout.preferredWidth: 84
                                Layout.fillHeight: true
                                radius: 16
                                color: "#162127"
                                border.color: "#233039"

                                Column {
                                    anchors.centerIn: parent
                                    spacing: 8
                                    Text {
                                        text: modelData.tag
                                        color: window.accent
                                        font.pixelSize: 11
                                        font.bold: true
                                        horizontalAlignment: Text.AlignHCenter
                                        width: parent.width
                                    }
                                    Text {
                                        text: modelData.time
                                        color: window.textDim
                                        font.pixelSize: 10
                                        horizontalAlignment: Text.AlignHCenter
                                        width: parent.width
                                    }
                                }
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                spacing: 8

                                Text {
                                    Layout.fillWidth: true
                                    text: modelData.title
                                    color: window.textPrimary
                                    font.pixelSize: 16
                                    font.bold: true
                                    wrapMode: Text.WordWrap
                                }

                                Text {
                                    Layout.fillWidth: true
                                    text: modelData.summary
                                    color: window.textMuted
                                    font.pixelSize: 12
                                    wrapMode: Text.WordWrap
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
