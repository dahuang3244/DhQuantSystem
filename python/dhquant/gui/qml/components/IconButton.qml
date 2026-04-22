import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    property string iconText: ""
    property color defaultColor: "transparent"
    property color hoverColor: "#273250"
    property color iconColor: "#97A3BF"
    property color iconHoverColor: "#F5F7FB"
    signal clicked()

    width: 32
    height: 32
    radius: 6
    color: mouseArea.containsMouse ? hoverColor : defaultColor

    Text {
        anchors.centerIn: parent
        text: root.iconText
        font.pixelSize: 14
        color: mouseArea.containsMouse ? iconHoverColor : iconColor
        Behavior on color { ColorAnimation { duration: 150 } }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
        cursorShape: Qt.PointingHandCursor
    }
    
    Behavior on color { ColorAnimation { duration: 150 } }
}
