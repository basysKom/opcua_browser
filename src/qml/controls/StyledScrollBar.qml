/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls

import OPC_UA_Browser

ScrollBar {
    id: control

    property ThemeScrollBar theme: Style.scrollBar

    contentItem: Rectangle {
        implicitWidth: 10
        implicitHeight: 10
        radius: 5
        color: theme.selector
        opacity: (control.policy === ScrollBar.AlwaysOn)
                 || (control.active && control.size < 1.0) ? 0.75 : 0

        // Animate the changes in opacity (default duration is 250 ms).
        Behavior on opacity {
            NumberAnimation {}
        }
    }

    background: Rectangle {
        color: theme.background
    }
}
