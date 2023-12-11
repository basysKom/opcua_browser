import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import OPC_UA_Browser

ColumnLayout {
    id: layout

    property int textColumnHeight: 15
    property int comboBoxColumnHeight: 30
    property alias enabled: comboBox.enabled
    property alias captionText: caption.text
    property alias model: comboBox.model
    property alias currentIndex: comboBox.currentIndex
    property ThemeComboBox theme: Style.comboBox

    Text {
        id: caption

        Layout.preferredHeight: layout.textColumnHeight
        verticalAlignment: Qt.AlignVCenter
        color: theme.captionTextColor
        font.bold: true
    }

    ComboBox {
        id: comboBox

        Layout.fillWidth: true
        Layout.preferredHeight: layout.comboBoxColumnHeight
        palette.button: theme.background
    }
}
