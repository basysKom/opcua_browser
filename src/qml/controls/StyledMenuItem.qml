/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls

import OPC_UA_Browser

MenuItem {
    id: control

    property alias iconSource: control.icon.source
    property ThemeSideMenu theme: Style.sideMenu

    visible: enabled
    height: enabled ? 40 : 0
    width: implicitWidth
    font.pointSize: 14
    palette.windowText: theme.textColor

    icon {
        color: theme.iconColor
        height: 24
        width: 24
    }

    background: Rectangle {
        color: control.theme.background
    }
}
