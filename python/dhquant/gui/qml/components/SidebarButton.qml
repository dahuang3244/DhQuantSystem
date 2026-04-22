import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    property string iconText: ""
    property string label: ""
    property bool isActive: false
    signal clicked()

    width: 64
    height: 64
    radius: 12
    color: isActive ? "#1A2642" : (mouseArea.containsMouse ? "#131A2E" : "transparent")
    border.color: isActive ? "#49C6A5" : "transparent"
    border.width: 1

    Behavior on color { ColorAnimation { duration: 250; easing.type: Easing.OutCubic } }
    Behavior on border.color { ColorAnimation { duration: 250; easing.type: Easing.OutCubic } }

    Column {
        anchors.centerIn: parent
        spacing: 6
        Text {
            text: root.iconText
            font.pixelSize: 22
            anchors.horizontalCenter: parent.horizontalCenter
            scale: root.isActive ? 1.15 : (mouseArea.containsMouse ? 1.08 : 1.0)
            Behavior on scale { NumberAnimation { duration: 250; easing.type: Easing.OutBack } }
        }
        Text {
            text: root.label
            color: root.isActive ? "#49C6A5" : (mouseArea.containsMouse ? "#F5F7FB" : "#97A3BF")
            font.pixelSize: 11
            font.bold: root.isActive
            anchors.horizontalCenter: parent.horizontalCenter
            Behavior on color { ColorAnimation { duration: 250; easing.type: Easing.OutCubic } }
        }
    }
    
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
        cursorShape: Qt.PointingHandCursor
    }
}
