import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    readonly property bool canSaveDashboard: dashboardTab.canSaveDashboard && (stackLayout.currentIndex === 1)

    /*TabBar {
        id: tabBar

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
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
    }*/
    StackLayout {
        id: stackLayout

        anchors.fill: parent
        visible: true

        currentIndex: 1

        BrowserView {
            id: browserTab

            onSelectionCancelled: stackLayout.currentIndex = 1
            onSelectionAccepted: stackLayout.currentIndex = 1
        }

        DashboardView {
            id: dashboardTab

            onAddMonitoredItems: {
                stackLayout.currentIndex = 0
                browserTab.type = BrowserView.Type.SelectMonitoredItem
            }
        }
    }
}
