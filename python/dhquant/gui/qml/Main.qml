import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "components"

ApplicationWindow {
    id: window
    width: 1480
    height: 920
    visible: true
    title: "DhQuant Workspace"
    color: bgPrimary
    
    // Frameless window: Removes macOS system navigation and title bar
    flags: Qt.Window | Qt.FramelessWindowHint

    // Global Theme Colors
    property color bgPrimary: "#0B1020"
    property color bgPanel: "#131A2E"
    property color bgPanelAlt: "#1A233B"
    property color borderColor: "#273250"
    property color textPrimary: "#F5F7FB"
    property color textMuted: "#97A3BF"
    property color accent: "#49C6A5"
    property color warn: "#F4B860"
    property color danger: "#EE6C70"
    
    function toneColor(tone) {
        if (tone === "good") return accent;
        if (tone === "warn") return warn;
        if (tone === "danger") return danger;
        if (tone === "accent") return "#58A6FF";
        return textPrimary;
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Custom Modern Title Bar
        CustomTitleBar {
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            // Left Sidebar Navigation
            Sidebar {
                Layout.fillHeight: true
                currentIndex: stackView.currentIndex
                onTabClicked: (index) => stackView.currentIndex = index
            }

            // Main Content Area with fading optimization animation
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Rectangle {
                    anchors.fill: parent
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#0A0F1E" }
                        GradientStop { position: 0.6; color: "#10172B" }
                        GradientStop { position: 1.0; color: "#131B31" }
                    }
                }

                StackLayout {
                    id: stackView
                    anchors.fill: parent
                    currentIndex: 0

                    FadeItem { MarketPage { anchors.fill: parent } }
                    FadeItem { NewsPage { anchors.fill: parent } }
                    FadeItem { BacktestPage { anchors.fill: parent } }
                    FadeItem {
                        Item {
                            anchors.fill: parent
                            Label {
                                text: "Settings (Under Construction)"
                                color: textPrimary
                                font.pixelSize: 24
                                anchors.centerIn: parent
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Component for adding custom page transition fade
    component FadeItem : Item {
        id: fadeRoot
        opacity: StackLayout.isCurrentItem ? 1.0 : 0.0
        visible: opacity > 0
        Behavior on opacity {
            NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
        }
    }
}
