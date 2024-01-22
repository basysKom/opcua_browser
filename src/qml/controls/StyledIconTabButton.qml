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

TabButton {
    id: control

    readonly property bool isCurrentTab: (TabBar.tabBar.currentIndex == TabBar.index)
                                         && (type !== DashboardItem.DashboardType.Add)

    property ThemeIconTabButton theme: Style.iconTabButton

    property int type: DashboardItem.DashboardType.Variables

    width: 80

    background: Rectangle {
        color: control.isCurrentTab ? control.theme.backgroundSelected : control.theme.background
    }

    contentItem: ColumnLayout {
        width: control.width
        spacing: 5

        IconImage {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            sourceSize.width: 40
            sourceSize.height: 40
            source: (DashboardItem.DashboardType.Add === control.type) ? "qrc:/icons/plus.svg" :
                    (DashboardItem.DashboardType.Events === control.type) ? "qrc:/icons/event.svg" : "qrc:/icons/dashboard.svg"
            color: label.color
        }

        Text {
            id: label

            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 8
            text: control.text
            color: control.isCurrentTab ? control.theme.textColorSelected : control.theme.textColor
            elide: Text.ElideRight
        }
    }
}
