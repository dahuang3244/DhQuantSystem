import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root
    height: 58
    color: "#0c1115"

    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
        color: window.line
    }

    MouseArea {
        anchors.fill: parent
        property point clickPos: Qt.point(0, 0)
        onDoubleClicked: {
            if (window.visibility === Window.Maximized)
                window.showNormal();
            else
                window.showMaximized();
        }
        onPressed: (mouse) => clickPos = Qt.point(mouse.x, mouse.y)
        onPositionChanged: (mouse) => {
            var dx = mouse.x - clickPos.x;
            var dy = mouse.y - clickPos.y;
            window.x += dx;
            window.y += dy;
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 18
        anchors.rightMargin: 14
        spacing: 16

        RowLayout {
            spacing: 10

            Rectangle {
                Layout.preferredWidth: 26
                Layout.preferredHeight: 26
                radius: 8
                color: window.accentSoft
                border.color: "#245747"

                Canvas {
                    anchors.fill: parent
                    onPaint: {
                        var ctx = getContext("2d");
                        ctx.reset();
                        ctx.strokeStyle = window.accent;
                        ctx.lineWidth = 1.6;
                        ctx.beginPath();
                        ctx.moveTo(5, 17);
                        ctx.lineTo(10, 12);
                        ctx.lineTo(14, 14);
                        ctx.lineTo(20, 7);
                        ctx.stroke();
                    }
                }
            }

            Column {
                spacing: 1
                Text {
                    text: "DhQuant"
                    color: window.textPrimary
                    font.pixelSize: 14
                    font.bold: true
                    font.letterSpacing: 0.6
                }
                Text {
                    text: "Systematic research workspace"
                    color: window.textDim
                    font.pixelSize: 10
                    font.letterSpacing: 0.4
                }
            }
        }

        Item { Layout.fillWidth: true }

        Rectangle {
            Layout.preferredWidth: 230
            Layout.preferredHeight: 34
            radius: 17
            color: "#12191f"
            border.color: window.line

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 12
                spacing: 8

                Rectangle {
                    Layout.preferredWidth: 8
                    Layout.preferredHeight: 8
                    radius: 4
                    color: window.accent
                    opacity: 0.9
                }

                Text {
                    Layout.fillWidth: true
                    text: "Research mode"
                    color: window.textMuted
                    font.pixelSize: 11
                }

                Text {
                    text: "SIM"
                    color: window.textPrimary
                    font.pixelSize: 11
                    font.bold: true
                }
            }
        }

        Row {
            spacing: 8

            Rectangle {
                width: 14; height: 14; radius: 7
                color: closeArea.containsMouse ? "#db5f52" : "#373737"
                MouseArea {
                    id: closeArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: window.close()
                }
            }
            Rectangle {
                width: 14; height: 14; radius: 7
                color: minArea.containsMouse ? "#d9a557" : "#373737"
                MouseArea {
                    id: minArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: window.showMinimized()
                }
            }
            Rectangle {
                width: 14; height: 14; radius: 7
                color: maxArea.containsMouse ? "#45c48d" : "#373737"
                MouseArea {
                    id: maxArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        if (window.visibility === Window.Maximized)
                            window.showNormal();
                        else
                            window.showMaximized();
                    }
                }
            }
        }
    }
}
