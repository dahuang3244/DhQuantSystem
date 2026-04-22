import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root
    width: 80
    color: "#0D1325"
    
    // Right border
    Rectangle {
        width: 1
        height: parent.height
        anchors.right: parent.right
        color: "#131A2E"
    }
    
    property int currentIndex: 0
    signal tabClicked(int index)

    Column {
        anchors.top: parent.top
        anchors.topMargin: 24
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 16

        // Market / K-Line Tab
        SidebarButton {
            iconText: "📊"
            label: "行情"
            isActive: root.currentIndex === 0
            onClicked: root.tabClicked(0)
        }
        
        // News Gathering Tab
        SidebarButton {
            iconText: "📰"
            label: "资讯"
            isActive: root.currentIndex === 1
            onClicked: root.tabClicked(1)
        }

        // Backtest Pipeline Tab
        SidebarButton {
            iconText: "🧪"
            label: "回测"
            isActive: root.currentIndex === 2
            onClicked: root.tabClicked(2)
        }
        
        // Settings Tab
        SidebarButton {
            iconText: "⚙️"
            label: "设置"
            isActive: root.currentIndex === 3
            onClicked: root.tabClicked(3)
        }
    }
}
