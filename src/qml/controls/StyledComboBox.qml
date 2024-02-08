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
    property int comboBoxColumnHeight: 36
    property alias enabled: comboBox.enabled
    property alias captionText: caption.text
    property alias model: comboBox.model
    property alias textRole: comboBox.textRole
    property alias currentIndex: comboBox.currentIndex
    property alias currentText: comboBox.currentText
    property alias _comboBox: comboBox
    property ThemeComboBox theme: Style.comboBox

    Text {
        id: caption

        Layout.preferredHeight: layout.textColumnHeight
        verticalAlignment: Qt.AlignVCenter
        color: layout.theme.captionTextColor
        font.bold: true
    }

    ComboBox {
        id: comboBox

        Layout.fillWidth: true
        Layout.preferredHeight: layout.comboBoxColumnHeight
        palette.button: layout.theme.background
    }
}
