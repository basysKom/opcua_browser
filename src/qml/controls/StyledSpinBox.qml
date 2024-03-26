/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import OPC_UA_Browser

ColumnLayout {
    id: layout

    property ThemeSpinBox theme: Style.spinBox
    property alias captionText: caption.text
    property alias from: spinBox.from
    property alias to: spinBox.to
    property alias value: spinBox.value

    Text {
        id: caption

        verticalAlignment: Qt.AlignVCenter
        color: layout.theme.textColor
        font.bold: true
    }

    SpinBox {
        id: spinBox
        height: 30

        background: Rectangle {
            implicitWidth: 100
            color: "transparent"
            border.color: "transparent"
            border.width: 0
        }

        contentItem: TextInput {
            z: 2
            text: spinBox.textFromValue(spinBox.value, spinBox.locale)

            font: spinBox.font
            color: theme.textColor
            selectionColor: theme.textBackgroundSelected
            selectedTextColor: theme.textColor
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter

            readOnly: !spinBox.editable
            validator: spinBox.validator
            inputMethodHints: Qt.ImhFormattedNumbersOnly
        }

        up.indicator: IconImage {
            x: spinBox.mirrored ? 0 : parent.width - width
            sourceSize.width: 30
            sourceSize.height: 30
            source: "qrc:/icons/plus.svg"
            color: spinBox.up.pressed || spinBox.to == value ? theme.buttonBackgroundSelected : theme.textColor
        }

        down.indicator: IconImage {
            x: spinBox.mirrored ? parent.width - width : 0
            sourceSize.width: 30
            sourceSize.height: 30
            source: "qrc:/icons/minus.svg"
            color: spinBox.down.pressed || spinBox.from == value ? theme.buttonBackgroundSelected : theme.textColor
        }
    }
}
