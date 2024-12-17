/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Controls.Fusion
import QtQuick.Controls.Fusion.impl

Rectangle {
    id: indicator

    property Item control
    property color indicatorBackgroundColor: control.palette.base
    readonly property color pressedColor: Fusion.mergedColors(control.palette.base, control.palette.windowText, 85)
    readonly property color checkMarkColor: Qt.darker(control.palette.highlight, 1.2)

    implicitWidth: 24
    implicitHeight: 24

    radius: width / 2
    color: control.down ? indicator.pressedColor : indicatorBackgroundColor

    border.width: 1
    border.color: control.palette.highlight

    ColorImage {
        anchors.centerIn: parent

        visible: indicator.control.checkState === Qt.Checked || (indicator.control.checked && indicator.control.checkState === undefined)
        fillMode: Image.PreserveAspectFit
        color: indicator.palette.highlight

        source: "qrc:/icons/checkmark.svg"
        sourceSize.width: parent.height * 0.65
        sourceSize.height: parent.width * 0.65
    }
}
