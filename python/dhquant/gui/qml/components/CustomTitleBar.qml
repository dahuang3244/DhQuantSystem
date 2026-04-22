import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root
    height: 42
    color: "#0A0F1E"
    
    // Bottom border
    Rectangle {
        width: parent.width
        height: 1
        anchors.bottom: parent.bottom
        color: "#1E293B"
    }
    
    // Double-click to maximize/restore; drag to move window
    MouseArea {
        anchors.fill: parent
        property point clickPos: Qt.point(0, 0)
        onDoubleClicked: {
            if (window.visibility === Window.Maximized)
                window.showNormal();
            else
                window.showMaximized();
        }
        onPressed: (mouse) => {
            clickPos = Qt.point(mouse.x, mouse.y);
        }
        onPositionChanged: (mouse) => {
            var dx = mouse.x - clickPos.x;
            var dy = mouse.y - clickPos.y;
            window.x += dx;
            window.y += dy;
        }
    }
    
    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 10
        spacing: 12
        
        // Logo and Title
        Row {
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            spacing: 8
            
            Text {
                text: "◈"
                font.pixelSize: 16
                color: "#2563EB"
                anchors.verticalCenter: parent.verticalCenter
            }
            Text {
                text: "DhQuant"
                color: "#F1F5F9"
                font.pixelSize: 13
                font.bold: true
                font.letterSpacing: 0.8
                anchors.verticalCenter: parent.verticalCenter
            }
            Text {
                text: "Workspace"
                color: "#64748B"
                font.pixelSize: 13
                font.letterSpacing: 0.5
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Item { Layout.fillWidth: true }
        
        // Window Controls (macOS-style dots)
        Row {
            spacing: 8
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

            // Close
            Rectangle {
                width: 14; height: 14; radius: 7
                color: closeMa.containsMouse ? "#EF4444" : "#3B3B3B"
                Behavior on color { ColorAnimation { duration: 120 } }
                Text {
                    anchors.centerIn: parent
                    text: "✕"
                    font.pixelSize: 8
                    color: closeMa.containsMouse ? "#FFFFFF" : "transparent"
                }
                MouseArea {
                    id: closeMa; anchors.fill: parent; hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: window.close()
                }
            }
            // Minimize
            Rectangle {
                width: 14; height: 14; radius: 7
                color: minMa.containsMouse ? "#F59E0B" : "#3B3B3B"
                Behavior on color { ColorAnimation { duration: 120 } }
                Text {
                    anchors.centerIn: parent
                    text: "−"
                    font.pixelSize: 10
                    color: minMa.containsMouse ? "#FFFFFF" : "transparent"
                }
                MouseArea {
                    id: minMa; anchors.fill: parent; hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: window.showMinimized()
                }
            }
            // Maximize
            Rectangle {
                width: 14; height: 14; radius: 7
                color: maxMa.containsMouse ? "#22C55E" : "#3B3B3B"
                Behavior on color { ColorAnimation { duration: 120 } }
                Text {
                    anchors.centerIn: parent
                    text: window.visibility === Window.Maximized ? "❐" : "□"
                    font.pixelSize: 8
                    color: maxMa.containsMouse ? "#FFFFFF" : "transparent"
                }
                MouseArea {
                    id: maxMa; anchors.fill: parent; hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
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
