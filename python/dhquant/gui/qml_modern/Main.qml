import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "components"
import "pages"

ApplicationWindow {
    id: window
    width: 1520
    height: 940
    visible: true
    title: "DhQuant Modern Workspace"
    color: shell
    flags: Qt.Window | Qt.FramelessWindowHint

    property color shell: "#0b0f12"
    property color surface: "#12181d"
    property color surfaceRaised: "#171f25"
    property color surfaceSoft: "#1d262d"
    property color line: "#27323b"
    property color lineSoft: "#33404a"
    property color textPrimary: "#f3f5f7"
    property color textMuted: "#8b99a6"
    property color textDim: "#62707d"
    property color accent: "#45c4a1"
    property color accentSoft: "#17362d"
    property color danger: "#ed6b5f"
    property color warn: "#d9a557"
    property color positive: "#e35d6a"
    property color negative: "#33b06e"

    property int currentIndex: 0

    function toneColor(tone) {
        if (tone === "good" || tone === "positive") return accent;
        if (tone === "warn") return warn;
        if (tone === "danger") return danger;
        if (tone === "accent") return "#73d9bd";
        return textPrimary;
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#090c0f" }
            GradientStop { position: 0.45; color: "#0c1115" }
            GradientStop { position: 1.0; color: "#11181d" }
        }
    }

    Rectangle {
        anchors.top: parent.top
        anchors.right: parent.right
        width: parent.width * 0.45
        height: parent.height * 0.35
        radius: width / 2
        color: "#123528"
        opacity: 0.16
        visible: true
        x: parent.width - width * 0.65
        y: -height * 0.5
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        TopBar {
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            AppSidebar {
                Layout.fillHeight: true
                currentIndex: window.currentIndex
                onNavigate: (index) => window.currentIndex = index
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true

                StackLayout {
                    anchors.fill: parent
                    currentIndex: window.currentIndex

                    FadePage { MarketPage { anchors.fill: parent } }
                    FadePage { NewsPage { anchors.fill: parent } }
                    FadePage { BacktestPage { anchors.fill: parent } }
                }
            }
        }
    }

    component FadePage: Item {
        opacity: StackLayout.isCurrentItem ? 1 : 0
        visible: opacity > 0
        Behavior on opacity {
            NumberAnimation { duration: 220; easing.type: Easing.InOutQuad }
        }
    }
}
