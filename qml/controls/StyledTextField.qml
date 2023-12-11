import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import OPC_UA_Browser

ColumnLayout {
    id: layout

    property int textColumnHeight: 15
    property int textFieldColumnHeight: 30
    property alias enabled: textField.enabled
    property alias captionText: caption.text
    property alias text: textField.text
    property alias placeholderText: textField.placeholderText
    property ThemeTextField theme: Style.textField

    Text {
        id: caption

        Layout.preferredHeight: layout.textColumnHeight
        verticalAlignment: Qt.AlignVCenter
        color: theme.captionTextColor
        font.bold: true
    }

    TextField {
        id: textField

        Layout.fillWidth: true
        Layout.preferredHeight: layout.textFieldColumnHeight
        verticalAlignment: Qt.AlignVCenter

        background: Rectangle {
            color: theme.background
        }
    }
}
