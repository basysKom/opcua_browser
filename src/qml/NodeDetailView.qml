/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import OPC_UA_Browser

Item {
    id: root

    property alias attributes: attributesTab.attributes
    property alias references: referencesTab.references

    signal popupRequested(name: string, valueText: string)

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

            model: [qsTranslate("ExpertMode", "Attributes"), qsTranslate("ExpertMode", "References")]
            StyledTabButton {
                required property string modelData
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

            onPopupRequested: function(name, valueText) {
                root.popupRequested(name, valueText)
            }
        }

        NodeReferenceList {
            id: referencesTab
        }
    }
}
