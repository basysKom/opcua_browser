

/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import OPC_UA_Browser

ColumnLayout {
    id: layout

    property int textColumnHeight: 15
    property int textFieldColumnHeight: 36
    property alias enabled: textField.enabled
    property alias captionText: caption.text
    property alias text: textField.text
    property alias placeholderText: textField.placeholderText
    property alias echoMode: textField.echoMode
    property alias inputMethodHints: textField.inputMethodHints
    property ThemeTextField theme: Style.textField
    property alias validator: textField.validator
    property alias acceptableInput: textField.acceptableInput
    property alias prefixText: prefixTextField.text

    Text {
        id: caption

        Layout.preferredHeight: layout.textColumnHeight
        verticalAlignment: Qt.AlignVCenter
        color: layout.theme.captionTextColor
        font.bold: true
    }

    RowLayout {
        Text {
            id: prefixTextField
            visible: text && text !== ""
            color: layout.theme.captionTextColor
        }

        TextField {
            id: textField

            Layout.fillWidth: true
            Layout.preferredHeight: layout.textFieldColumnHeight
            verticalAlignment: Qt.AlignVCenter

            passwordCharacter: "*"
            passwordMaskDelay: 500

            background: Rectangle {
                color: layout.theme.background
            }
        }
    }
}
