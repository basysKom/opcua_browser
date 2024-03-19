/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl // IconImage
import QtQuick.Layouts

import OPC_UA_Browser

Popup {
    id: menu

    property alias listModel: popupListView.model
    property ThemeContextMenu theme: Style.contextMenu

    implicitWidth: contentItem.childrenRect.width
    implicitHeight: contentItem.childrenRect.height
    padding: 0

    modal: true

    signal listItemClicked(int index)

    background: Rectangle {
        id: transparentBorderRect

        width: menu.width
        height: menu.height
        radius: 3
        color: menu.theme.background
    }

    contentItem: Item {
        implicitWidth: popupListView.contentWidth
        implicitHeight: popupListView.contentHeight

        ListView {
            id: popupListView

            implicitWidth: contentItem.childrenRect.width
            implicitHeight: contentItem.childrenRect.height
            clip: true

            delegate: Item {
                id: delegateItem

                height: 36
                width: 200

                required property int index
                required property url imageSource
                required property string name

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true

                    onEntered: popupListView.currentIndex = delegateItem.index
                    onClicked: {
                        menu.listItemClicked(delegateItem.index)
                        menu.close()
                    }
                }

                Rectangle {
                    anchors.fill: parent
                    radius: transparentBorderRect.radius
                    color: menu.theme.backgroundSelected
                    opacity: popupListView.currentIndex === delegateItem.index ? 0.8 : 0
                }

                RowLayout {
                    anchors.fill: parent
                    spacing: 15

                    IconImage {
                        Layout.leftMargin: 10
                        Layout.alignment: Qt.AlignVCenter
                        sourceSize.width: 24
                        sourceSize.height: 24
                        source: delegateItem.imageSource
                        color: menu.theme.textColor
                    }
                    Text {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignLeft
                        color: menu.theme.textColor
                        font.pointSize: 12
                        text: delegateItem.name
                    }
                }
            }
        }
    }
}
