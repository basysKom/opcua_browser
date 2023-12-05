import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    TabBar {
        id: tabBar

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        visible: BackEnd.isConnected
        spacing: 0

        background: Rectangle {
            color: "transparent"
        }

        Repeater {
            id: repeater

            model: [qsTr("Browser"), qsTr("Dashboard")]
            StyledTabButton {
                text: modelData
                width: Math.max(100, root.width / repeater.count)
            }
        }
    }

    StackLayout {
        id: stackLayout

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: tabBar.bottom
        anchors.bottom: parent.bottom
        visible: tabBar.visible

        currentIndex: tabBar.currentIndex

        BrowserView {
            id: browserTab
        }

        DashboardView {
            id: dashboardTab
        }
    }
}
