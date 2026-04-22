pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: popup
    width: 720
    height: 520
    modal: true
    dim: true
    anchors.centerIn: parent
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    padding: 0

    property string marketName: "A股"
    property var stockData: []
    signal stockSelected(string code, string name, string market)

    property string searchText: ""
    property var filteredData: {
        if (!searchText || searchText.length === 0) return stockData;
        var q = searchText.toLowerCase();
        var result = [];
        for (var i = 0; i < stockData.length; i++) {
            var s = stockData[i];
            if (s.code.toLowerCase().indexOf(q) >= 0 ||
                s.name.toLowerCase().indexOf(q) >= 0 ||
                (s.industry && s.industry.toLowerCase().indexOf(q) >= 0)) {
                result.push(s);
            }
        }
        return result;
    }

    background: Rectangle {
        color: "#1a2332"
        radius: 12
        border.color: "#2a3a4a"
        border.width: 1
    }

    Overlay.modal: Rectangle {
        color: "#80000000"
    }

    contentItem: ColumnLayout {
        spacing: 0

        // Header
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            color: "#1e2d3d"
            radius: 12

            // Bottom corners not rounded
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 12
                color: parent.color
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 18
                anchors.rightMargin: 14

                Text {
                    text: popup.marketName + "股票列表 (" + popup.stockData.length + ")"
                    color: "#f3f5f7"
                    font.pixelSize: 15
                    font.bold: true
                }

                Item { Layout.fillWidth: true }

                Rectangle {
                    width: 28; height: 28; radius: 14
                    color: closeMA.containsMouse ? "#2a3a4a" : "transparent"
                    Text {
                        anchors.centerIn: parent
                        text: "✕"
                        color: "#8b99a6"
                        font.pixelSize: 14
                    }
                    MouseArea {
                        id: closeMA
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: popup.close()
                    }
                }
            }
        }

        // Search bar
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 44
            Layout.leftMargin: 16
            Layout.rightMargin: 16
            Layout.topMargin: 10
            radius: 8
            color: "#141e28"
            border.color: "#2a3a4a"

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 12

                Text {
                    text: "🔍"
                    font.pixelSize: 13
                    color: "#62707d"
                }

                TextInput {
                    Layout.fillWidth: true
                    color: "#f3f5f7"
                    font.pixelSize: 13
                    clip: true
                    onTextChanged: popup.searchText = text

                    Text {
                        anchors.fill: parent
                        anchors.verticalCenter: parent.verticalCenter
                        text: "搜索代码、名称或行业..."
                        color: "#4a5a6a"
                        font.pixelSize: 13
                        visible: !parent.text
                    }
                }
            }
        }

        // Table header
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 36
            Layout.topMargin: 8
            color: "#162030"

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 18
                anchors.rightMargin: 18
                spacing: 0

                Text { text: "代码"; color: "#62707d"; font.pixelSize: 11; font.bold: true; Layout.preferredWidth: 80 }
                Text { text: "名称"; color: "#62707d"; font.pixelSize: 11; font.bold: true; Layout.preferredWidth: 90 }
                Text { text: "交易所"; color: "#62707d"; font.pixelSize: 11; font.bold: true; Layout.preferredWidth: 70 }
                Text { text: "板块"; color: "#62707d"; font.pixelSize: 11; font.bold: true; Layout.preferredWidth: 70 }
                Text { text: "状态"; color: "#62707d"; font.pixelSize: 11; font.bold: true; Layout.preferredWidth: 70 }
                Text { text: "行业"; color: "#62707d"; font.pixelSize: 11; font.bold: true; Layout.fillWidth: true }
                Text { text: "上市日期"; color: "#62707d"; font.pixelSize: 11; font.bold: true; Layout.preferredWidth: 90; horizontalAlignment: Text.AlignRight }
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: "#2a3a4a" }

        // Stock list
        ListView {
            id: stockListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: popup.filteredData
            boundsBehavior: Flickable.StopAtBounds

            delegate: Rectangle {
                id: rowDelegate
                required property var modelData
                required property int index
                width: stockListView.width
                height: 36
                color: rowMA.containsMouse ? "#1e2d3d" : (index % 2 === 0 ? "#1a2332" : "#182030")

                Behavior on color { ColorAnimation { duration: 100 } }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 18
                    anchors.rightMargin: 18
                    spacing: 0

                    Text { text: rowDelegate.modelData.code; color: "#5a9cf5"; font.pixelSize: 12; font.family: "Menlo"; Layout.preferredWidth: 80 }
                    Text { text: rowDelegate.modelData.name; color: "#f3f5f7"; font.pixelSize: 12; Layout.preferredWidth: 90; elide: Text.ElideRight }
                    Text { text: rowDelegate.modelData.exchange || ""; color: "#8b99a6"; font.pixelSize: 11; Layout.preferredWidth: 70 }
                    Text { text: rowDelegate.modelData.board || ""; color: "#8b99a6"; font.pixelSize: 11; Layout.preferredWidth: 70 }
                    Text {
                        text: rowDelegate.modelData.status || "Normal"
                        color: rowDelegate.modelData.status === "*ST" ? "#ed6b5f" : "#45c4a1"
                        font.pixelSize: 11
                        Layout.preferredWidth: 70
                    }
                    Text { text: rowDelegate.modelData.industry || ""; color: "#8b99a6"; font.pixelSize: 11; Layout.fillWidth: true; elide: Text.ElideRight }
                    Text { text: rowDelegate.modelData.listDate || ""; color: "#8b99a6"; font.pixelSize: 11; Layout.preferredWidth: 90; horizontalAlignment: Text.AlignRight }
                }

                MouseArea {
                    id: rowMA
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        popup.stockSelected(rowDelegate.modelData.code, rowDelegate.modelData.name, popup.marketName);
                        popup.close();
                    }
                }
            }

            ScrollBar.vertical: ScrollBar {
                active: true
                policy: ScrollBar.AsNeeded
            }
        }
    }
}
