/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls

import OPC_UA_Browser

Button {
    id: button

    highlighted: true
    implicitHeight: 36

    property ThemeButton theme: Style.button
    property int radius: 5

    palette.brightText: theme.highlightedTextColor
    palette.buttonText: theme.textColor

    background: Rectangle {
        color: button.highlighted ? button.theme.highlightedBackground : button.theme.background
        border.color: button.highlighted ? button.theme.highlightedBorderColor : button.theme.borderColor
        border.width: 1
        radius: button.radius
    }
}
