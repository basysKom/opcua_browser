/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl // IconImage

import OPC_UA_Browser

CheckBox {
    id: control

    property ThemeItemSelector theme: Style.itemSelector

    indicator: null
    background: Rectangle {
        id: background
        width: control.width
        height: width
        radius: width / 2
        color: control.theme.background
        opacity: control.theme.backgroundOpacity
    }

    Rectangle {
        anchors.fill: parent
        radius: background.radius
        border.width: 1
        border.color: control.theme.borderColor
        color: "transparent"

        IconImage {
            id: checkmark
            fillMode: Image.PreserveAspectFit
            anchors.centerIn: parent
            source: "qrc:/icons/checkmark.svg"
            sourceSize.width: parent.height * 0.65
            sourceSize.height: parent.width * 0.65
            color: control.theme.checkMarkColor
            visible: (control.checkState === Qt.Checked)
        }
    }
}
