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
    highlighted: true
    height: 36

    property ThemeButton theme: Style.button

    palette.brightText: theme.highlightedTextColor
    palette.buttonText: theme.textColor

    background: Rectangle {
        color: highlighted ? theme.highlightedBackground : theme.background
        border.color: highlighted ? theme.highlightedBorderColor : theme.borderColor
        border.width: 1
        radius: 5
    }
}
