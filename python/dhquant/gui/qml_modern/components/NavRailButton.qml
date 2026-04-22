import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    property string label: ""
    property string subLabel: ""
    property string iconType: "market"
    property bool active: false
    signal clicked()

    height: 76
    width: parent ? parent.width : 82
    radius: 20
    color: active ? "#151d22" : (area.containsMouse ? "#11171c" : "transparent")
    border.color: active ? "#2c4f46" : "transparent"

    Behavior on color {
        ColorAnimation { duration: 160 }
    }

    Column {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 8

        Rectangle {
            width: 34
            height: 34
            radius: 11
            color: active ? "#19362f" : "#141a1f"
            border.color: active ? "#275346" : "#212a31"

            Canvas {
                anchors.fill: parent
                onPaint: {
                    var ctx = getContext("2d");
                    ctx.reset();
                    ctx.strokeStyle = root.active ? window.accent : window.textMuted;
                    ctx.lineWidth = 1.7;
                    ctx.lineCap = "round";
                    if (root.iconType === "market") {
                        ctx.beginPath();
                        ctx.moveTo(7, 21);
                        ctx.lineTo(12, 16);
                        ctx.lineTo(17, 18);
                        ctx.lineTo(26, 10);
                        ctx.stroke();
                    } else if (root.iconType === "news") {
                        ctx.strokeRect(8, 9, 18, 16);
                        ctx.beginPath();
                        ctx.moveTo(12, 13);
                        ctx.lineTo(22, 13);
                        ctx.moveTo(12, 17);
                        ctx.lineTo(22, 17);
                        ctx.moveTo(12, 21);
                        ctx.lineTo(18, 21);
                        ctx.stroke();
                    } else {
                        ctx.beginPath();
                        ctx.moveTo(9, 22);
                        ctx.lineTo(9, 12);
                        ctx.lineTo(15, 12);
                        ctx.lineTo(15, 18);
                        ctx.lineTo(21, 18);
                        ctx.lineTo(21, 10);
                        ctx.lineTo(27, 10);
                        ctx.stroke();
                    }
                }
            }
        }

        Text {
            text: label
            color: active ? window.textPrimary : window.textMuted
            font.pixelSize: 12
            font.bold: active
        }

        Text {
            text: subLabel
            color: window.textDim
            font.pixelSize: 10
        }
    }

    MouseArea {
        id: area
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: root.clicked()
    }
}
