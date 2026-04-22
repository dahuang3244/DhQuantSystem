pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: popup
    width: 320
    height: Math.min(420, 52 + strategyList.length * 42 + 54)
    modal: true
    dim: true
    anchors.centerIn: parent
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    padding: 0

    property var strategyList: []
    property int selectedIndex: 0
    signal strategySelected(int index)
    signal createRequested()

    background: Rectangle {
        color: "#1a2332"
        radius: 12
        border.color: "#2a3a4a"
    }

    Overlay.modal: Rectangle {
        color: "#60000000"
    }

    contentItem: ColumnLayout {
        spacing: 0

        // Header
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 44
            color: "#1e2d3d"
            radius: 12
            Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 12; color: parent.color }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 16
                anchors.rightMargin: 12

                Text {
                    text: "选择策略 (" + popup.strategyList.length + ")"
                    color: "#f3f5f7"
                    font.pixelSize: 14
                    font.bold: true
                }
                Item { Layout.fillWidth: true }
                Rectangle {
                    width: 26; height: 26; radius: 13
                    color: clsMA.containsMouse ? "#2a3a4a" : "transparent"
                    Text { anchors.centerIn: parent; text: "✕"; color: "#8b99a6"; font.pixelSize: 13 }
                    MouseArea { id: clsMA; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: popup.close() }
                }
            }
        }

        // Strategy items
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: popup.strategyList
            boundsBehavior: Flickable.StopAtBounds

            delegate: Rectangle {
                id: del
                required property string modelData
                required property int index
                width: parent ? parent.width : 0
                height: 42
                color: del.index === popup.selectedIndex ? "#253545" : (sma.containsMouse ? "#1e2d3d" : "transparent")
                Behavior on color { ColorAnimation { duration: 100 } }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 16
                    anchors.rightMargin: 16

                    Rectangle {
                        width: 6; height: 6; radius: 3
                        color: del.index === popup.selectedIndex ? "#45c4a1" : "#3a4a5a"
                    }
                    Text {
                        text: del.modelData
                        color: del.index === popup.selectedIndex ? "#f3f5f7" : "#8b99a6"
                        font.pixelSize: 12
                        Layout.fillWidth: true
                    }
                }

                MouseArea {
                    id: sma
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: { popup.strategySelected(del.index); popup.close() }
                }
            }
        }

        // Create button
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 46
            Layout.leftMargin: 12
            Layout.rightMargin: 12
            Layout.bottomMargin: 8
            radius: 10
            color: crtMA.containsMouse ? "#253545" : "#1e2d3d"
            border.color: "#2a3a4a"
            Behavior on color { ColorAnimation { duration: 100 } }

            Row {
                anchors.centerIn: parent
                spacing: 8
                Text { text: "+"; color: "#45c4a1"; font.pixelSize: 16; font.bold: true }
                Text { text: "创建新策略"; color: "#45c4a1"; font.pixelSize: 12; font.bold: true }
            }

            MouseArea {
                id: crtMA
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: { popup.createRequested(); popup.close() }
            }
        }
    }
}
