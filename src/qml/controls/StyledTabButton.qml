/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls

import OPC_UA_Browser

TabButton {
    id: root

    readonly property bool isCurrentTab: (TabBar.tabBar.currentIndex == TabBar.index)

    property ThemeTabButton theme: Style.tabButton

    background: Rectangle {
        color: theme.background

        Rectangle {
            id: divider
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 4
            color: theme.dividerColor
        }

        Rectangle {
            anchors.centerIn: divider
            width: divider.width / 2
            height: 4
            visible: isCurrentTab
            color: theme.dividerColorSelected
        }
    }

    contentItem: Text {
        text: root.text
        font {
            pointSize: 11
            bold: true
            capitalization: Font.AllUppercase
        }
        color: theme.textColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
}
