import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 20

        Label {
            text: "策略回测引擎 (Pipeline 模式)"
            color: window.textPrimary
            font.pixelSize: 28
            font.bold: true
        }
        
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 20

            // Left panel: Pipeline Configuration
            Rectangle {
                Layout.preferredWidth: 380
                Layout.fillHeight: true
                radius: 12
                color: window.bgPanel
                border.color: window.borderColor

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 16

                    Label { text: "Pipeline 组装"; color: window.textPrimary; font.pixelSize: 18; font.bold: true }

                    // Pipeline Stage 1: Data Source
                    PipelineStageNode { stageName: "数据读取 (Data Source)"; stageValue: "CSV / Parquet Reader" }
                    
                    // Pipeline Stage 2: Event Bridge / Filter
                    PipelineStageNode { stageName: "事件过滤 (Event Bridge)"; stageValue: "行情清洗与重采样" }
                    
                    // Pipeline Stage 3: Strategy
                    PipelineStageNode { stageName: "交易策略 (Alpha)"; stageValue: "Dual Thrust 动量突破" }
                    
                    // Pipeline Stage 4: Execution / Matching
                    PipelineStageNode { stageName: "订单撮合 (Matcher)"; stageValue: "T+1 / 滑点模型" }

                    Item { Layout.fillHeight: true } // spacer

                    Button {
                        Layout.fillWidth: true
                        height: 48
                        text: "▶ 运行回测 Pipeline"
                        background: Rectangle {
                            color: window.accent
                            radius: 8
                        }
                        contentItem: Text {
                            text: parent.text
                            color: "#0B1020"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }

            // Right panel: Backtest Results
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 12
                color: window.bgPanel
                border.color: window.borderColor

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 16

                    Label { text: "执行结果与分析"; color: window.textPrimary; font.pixelSize: 18; font.bold: true }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        // KPI Cards
                        Repeater {
                            model: [
                                {"label": "年化收益", "value": "12.4%", "tone": "good"},
                                {"label": "最大回撤", "value": "8.5%", "tone": "warn"},
                                {"label": "夏普比率", "value": "1.82", "tone": "accent"}
                            ]
                            
                            Rectangle {
                                Layout.fillWidth: true
                                height: 80
                                radius: 8
                                color: window.bgPanelAlt
                                border.color: window.borderColor
                                
                                Column {
                                    anchors.centerIn: parent
                                    Label { text: modelData.label; color: window.textMuted }
                                    Label { text: modelData.value; color: window.toneColor(modelData.tone); font.pixelSize: 24; font.bold: true }
                                }
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        radius: 8
                        color: window.bgPanelAlt
                        border.color: window.borderColor
                        
                        Label {
                            anchors.centerIn: parent
                            text: "净值走势图表 (PlaceHolder)"
                            color: window.textMuted
                        }
                    }
                }
            }
        }
    }

    component PipelineStageNode: Rectangle {
        property string stageName: ""
        property string stageValue: ""
        
        Layout.fillWidth: true
        height: 70
        radius: 8
        color: window.bgPrimary
        border.color: window.borderColor
        
        RowLayout {
            anchors.fill: parent
            anchors.margins: 12
            
            Rectangle {
                width: 4
                height: parent.height
                color: window.accent
                radius: 2
            }
            
            Column {
                Layout.fillWidth: true
                spacing: 4
                Label { text: stageName; color: window.textMuted; font.pixelSize: 12 }
                Label { text: stageValue; color: window.textPrimary; font.bold: true }
            }
            
            Label { text: "⚙️"; color: window.textMuted }
        }
    }
}
