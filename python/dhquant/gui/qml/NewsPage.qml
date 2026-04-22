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
            text: "全网市场资讯与情绪监控"
            color: window.textPrimary
            font.pixelSize: 28
            font.bold: true
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 12
            
            TextField {
                Layout.fillWidth: true
                placeholderText: "搜索股票代码、资讯关键词..."
                background: Rectangle {
                    color: window.bgPanelAlt
                    radius: 8
                    border.color: window.borderColor
                }
                color: window.textPrimary
            }
            
            Button {
                text: "AI 分析"
                background: Rectangle {
                    color: window.accent
                    radius: 8
                }
                contentItem: Text {
                    text: parent.text
                    color: "#0B1020"
                    font.bold: true
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 20

            // Left panel: Sentiment
            Rectangle {
                Layout.preferredWidth: 350
                Layout.fillHeight: true
                radius: 12
                color: window.bgPanel
                border.color: window.borderColor

                Column {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 20

                    Label { text: "市场恐慌/贪婪指数"; color: window.textPrimary; font.pixelSize: 16 }
                    
                    Rectangle {
                        width: 200; height: 200
                        radius: 100
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: "transparent"
                        border.color: "#352857"
                        border.width: 15
                        
                        Label {
                            anchors.centerIn: parent
                            text: "48"
                            font.pixelSize: 64
                            font.bold: true
                            color: "#B38BFF"
                        }
                        Label {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: parent.verticalCenter
                            anchors.topMargin: 30
                            text: "中性态度"
                            color: window.textMuted
                        }
                    }

                    Label {
                        text: "关键趋势洞察 (AI 生成)"
                        color: window.textPrimary
                        font.pixelSize: 16
                        font.bold: true
                    }
                    
                    Text {
                        width: parent.width
                        text: "今日市场处于震荡整理局势。技术面均线系统聚拢，方向不明。建议多看少动，等待更明确的破局信号。"
                        color: window.textMuted
                        wrapMode: Text.Wrap
                        lineHeight: 1.5
                    }
                }
            }

            // Right panel: News List
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 12
                color: window.bgPanel
                border.color: window.borderColor

                ListView {
                    anchors.fill: parent
                    anchors.margins: 16
                    clip: true
                    spacing: 12
                    model: 10 // Mock news count

                    delegate: Rectangle {
                        width: ListView.view.width
                        height: 90
                        radius: 8
                        color: window.bgPanelAlt
                        
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 12
                            spacing: 4
                            
                            RowLayout {
                                Layout.fillWidth: true
                                Label { text: "[行业深度] 2026中国半导体分析报告"; color: window.textPrimary; font.bold: true; font.pixelSize: 16; Layout.fillWidth: true }
                                Label { text: "12:0" + index; color: window.textMuted }
                            }
                            
                            Label {
                                Layout.fillWidth: true
                                text: "五大晶圆厂近期产能爬坡，新能源与人工智能需求持续带动订单。整体表现超出预期..."
                                color: window.textMuted
                                elide: Text.ElideRight
                            }
                        }
                    }
                }
            }
        }
    }
}
