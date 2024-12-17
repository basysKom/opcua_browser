/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

T.SpinBox {
    id: control

    // Note: the width of the indicators are calculated into the padding
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentItem.implicitWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             up.implicitIndicatorHeight, down.implicitIndicatorHeight)

    leftPadding: padding + (control.mirrored ? (up.indicator ? up.indicator.width : 0) : (down.indicator ? down.indicator.width : 0))
    rightPadding: padding + (control.mirrored ? (down.indicator ? down.indicator.width : 0) : (up.indicator ? up.indicator.width : 0))

    validator: IntValidator {
        locale: control.locale.name
        bottom: Math.min(control.from, control.to)
        top: Math.max(control.from, control.to)
    }

    contentItem: TextInput {
        z: 2
        text: control.displayText

        font: spinBox.font
        color: control.palette.windowText
        selectionColor: control.palette.highlight
        selectedTextColor: control.palette.highlightedText
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter

        readOnly: !control.editable
        validator: control.validator
        inputMethodHints: Qt.ImhFormattedNumbersOnly
    }

    up.indicator: IconImage {
        x: control.mirrored ? 0 : parent.width - width
        height: control.height

        source: "qrc:/icons/plus.svg"
        sourceSize.width: height
        sourceSize.height: height

        color: control.up.pressed || control.to == value ? control.palette.mid : control.palette.text
    }

    down.indicator: IconImage {
        x: control.mirrored ? parent.width - width : 0
        height: control.height

        source: "qrc:/icons/minus.svg"
        sourceSize.width: height
        sourceSize.height: height

        color: control.down.pressed || control.from == value ? control.palette.mid : control.palette.text
    }

    background: Rectangle {
        implicitWidth: 100
        color: "transparent"
    }
}
