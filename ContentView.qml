import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {    
    TabBar {
        id: tabBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        visible: BackEnd.isConnected

        background: Rectangle {
            color: "transparent"
        }

        Repeater {
            model: ["Browser", "Dashboard"/*, "Log"*/]
            StyledTabButton { text: modelData }
        }
    }

    StackLayout {
        id: stackLayout
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: tabBar.bottom
        anchors.bottom: parent.bottom
        visible: BackEnd.isConnected

        currentIndex: tabBar.currentIndex
        BrowserView {
            id: browserTab
        }
        DashboardView {
            id: dashboardTab
        }
    }
}
