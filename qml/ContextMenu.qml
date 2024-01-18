/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import OPC_UA_Browser

Popup {
    id: contextMenu

    property alias listModel: popupListView.model
    property ThemeContextMenu theme: Style.contextMenu

    implicitWidth: contentItem.childrenRect.width
    implicitHeight: contentItem.childrenRect.height
    padding: 0

    modal: true

    signal listItemClicked(int index)

    background: Rectangle {
        id: transparentBorderRect

        width: contextMenu.width
        height: contextMenu.height
        radius: 3
        color: theme.background
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
                height: 36
                width: 200

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true

                    onEntered: popupListView.currentIndex = index
                    onClicked: {
                        listItemClicked(index)
                        contextMenu.close()
                    }
                }

                Rectangle {
                    anchors.fill: parent
                    radius: transparentBorderRect.radius
                    color: theme.backgroundSelected
                    opacity: popupListView.currentIndex === index ? 0.8 : 0
                }

                RowLayout {
                    anchors.fill: parent
                    spacing: 15

                    IconImage {
                        Layout.leftMargin: 10
                        Layout.alignment: Qt.AlignVCenter
                        sourceSize.width: 24
                        sourceSize.height: 24
                        source: model.imageSource
                        color: theme.textColor
                    }
                    Text {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHLeft
                        color: theme.textColor
                        font.pointSize: 12
                        text: model.name
                    }
                }
            }
        }
    }
}
