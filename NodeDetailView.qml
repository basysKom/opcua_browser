import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    property alias attributes: attributesTab.attributes
    property alias references: referencesTab.references
    property alias monitoredAttributes: monitoringTab.monitoredAttributes

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
            model: ["Attributes", "References", "Monitoring"/*, "Log"*/]
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
        height: 200

        currentIndex: tabBar.currentIndex
        NodeAttributeList {
            id: attributesTab
        }
        NodeReferenceList {
            id: referencesTab
        }
        NodeMonitoringList {
            id: monitoringTab
        }
    }
}
