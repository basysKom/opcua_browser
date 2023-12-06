import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import OPC_UA_Browser

Item {
    id: root

    property alias attributes: attributesTab.attributes
    property alias references: referencesTab.references

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

            model: [qsTr("Attributes"), qsTr("References")]
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
        visible: BackEnd.isConnected
        height: 200

        currentIndex: tabBar.currentIndex

        NodeAttributeList {
            id: attributesTab
        }

        NodeReferenceList {
            id: referencesTab
        }
    }
}
