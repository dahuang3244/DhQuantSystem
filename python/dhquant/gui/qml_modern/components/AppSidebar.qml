import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    width: 118
    color: "#0a0e11"
    property int currentIndex: 0
    signal navigate(int index)

    Rectangle {
        anchors.right: parent.right
        width: 1
        height: parent.height
        color: window.line
    }

    Column {
        anchors.left: parent.left
        anchors.leftMargin: 18
        anchors.right: parent.right
        anchors.rightMargin: 18
        anchors.top: parent.top
        anchors.topMargin: 26
        spacing: 10

        Text {
            text: "Desk"
            color: window.textDim
            font.pixelSize: 10
            font.letterSpacing: 1.8
        }

        NavRailButton {
            label: "Market"
            subLabel: "行情"
            iconType: "market"
            active: root.currentIndex === 0
            onClicked: root.navigate(0)
        }

        NavRailButton {
            label: "News"
            subLabel: "资讯"
            iconType: "news"
            active: root.currentIndex === 1
            onClicked: root.navigate(1)
        }

        NavRailButton {
            label: "Lab"
            subLabel: "回测"
            iconType: "backtest"
            active: root.currentIndex === 2
            onClicked: root.navigate(2)
        }
    }

    Text {
        anchors.left: parent.left
        anchors.leftMargin: 18
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 22
        text: "v2"
        color: window.textDim
        font.pixelSize: 10
        font.letterSpacing: 1.4
    }
}
