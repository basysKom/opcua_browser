/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Controls.Fusion
import QtQuick.Templates as T

T.CheckBox {
    id: control

    property color indicatorBackgroundColor: control.palette.base

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    padding: 0
    spacing: 6

    font.pointSize: 12

    indicator: Rectangle {
        readonly property color pressedColor: Fusion.mergedColors(control.palette.base, control.palette.windowText, 85)
        readonly property color checkMarkColor: Qt.darker(control.palette.highlight, 1.2)

        implicitWidth: 24
        implicitHeight: 24

        x: control.text ? (control.mirrored ? control.width - width - control.rightPadding : control.leftPadding) : control.leftPadding + (control.availableWidth - width) / 2
        y: control.topPadding + (control.availableHeight - height) / 2

        radius: width / 2

        color: control.down ? control.palette.light : control.indicatorBackgroundColor
        border.width: 1
        border.color: control.palette.highlight

        ColorImage {
            anchors.centerIn: parent

            visible: control.checkState === Qt.Checked || (control.checked && control.checkState === undefined)
            fillMode: Image.PreserveAspectFit
            color: control.palette.highlight

            source: "qrc:/icons/checkmark.svg"
            sourceSize.width: parent.height * 0.65
            sourceSize.height: parent.width * 0.65
        }
    }

    contentItem: Text {
        leftPadding: control.indicator && !control.mirrored ? control.indicator.width + control.spacing : 0
        rightPadding: control.indicator && control.mirrored ? control.indicator.width + control.spacing : 0

        text: control.text
        font: control.font
        color: control.palette.windowText
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
    }
}
