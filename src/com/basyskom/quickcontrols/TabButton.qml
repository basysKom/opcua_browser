/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Templates as T

T.TabButton {
    id: control

    readonly property bool isCurrentTab: (T.TabBar.tabBar !== null) && (T.TabBar.tabBar.currentIndex === T.TabBar.index)

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 6
    spacing: 6

    icon.width: 24
    icon.height: 24
    icon.color: checked ? palette.windowText : palette.brightText

    contentItem: Text {
        text: control.text

        font {
            pointSize: 11
            bold: true
            capitalization: Font.AllUppercase
        }

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight

        color: control.palette.windowText
    }

    background: Item {
        implicitHeight: 40

        Rectangle {
            id: divider
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 4
            color: palette.mid
        }

        Rectangle {
            anchors.centerIn: divider
            width: divider.width / 2
            height: 4
            visible: control.isCurrentTab
            color: palette.highlight
        }
    }
}
