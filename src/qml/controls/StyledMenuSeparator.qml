/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls

import OPC_UA_Browser

MenuSeparator {
    id: separator

    property ThemeSideMenu theme: Style.sideMenu

    height: enabled ? implicitHeight : 0

    contentItem: Rectangle {
        implicitHeight: 1
        color: separator.theme.iconColor
    }
}
